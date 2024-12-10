#define _DEFAULT_SOURCE
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <signal.h>

#define MAX_COMMAND_LINE_LEN 1024
#define MAX_COMMAND_LINE_ARGS 128

extern char **environ;  // Access to environment variables

pid_t fg_process_pid = -1;  // Track the foreground process PID

// Function prototypes
bool is_builtin_command(char *cmd);
void execute_builtin_commands(char *arguments[]);
void execute_command(char *arguments[], bool is_background);

// Function to get the prompt with the current working directory
char *get_prompt() {
    static char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        strcat(cwd, "> ");
        return cwd;
    } else {
        perror("getcwd error");
        exit(1);
    }
}

// Signal handler for SIGINT (Ctrl-C) to prevent the shell from quitting
void handle_sigint(int sig) {
    printf("\nCaught Ctrl-C. Returning to shell prompt...\n");
    printf("%s", get_prompt());
    fflush(stdout);
}

// Signal handler for SIGALRM to kill long-running foreground processes
void handle_sigalrm(int sig) {
    if (fg_process_pid > 0) {
        printf("\nForeground process timed out. Killing PID: %d\n", fg_process_pid);
        kill(fg_process_pid, SIGKILL);
        fg_process_pid = -1;
    }
}

// Function to tokenize the input command line into arguments
void tokenize_command(char *command_line, char *arguments[]) {
    int arg_index = 0;
    char *token = strtok(command_line, " \t\r\n");
    while (token != NULL && arg_index < MAX_COMMAND_LINE_ARGS - 1) {
        arguments[arg_index++] = token;
        token = strtok(NULL, " \t\r\n");
    }
    arguments[arg_index] = NULL;
}

// Check if the command is a built-in command
bool is_builtin_command(char *cmd) {
    return strcmp(cmd, "cd") == 0 || strcmp(cmd, "pwd") == 0 ||
           strcmp(cmd, "echo") == 0 || strcmp(cmd, "env") == 0 ||
           strcmp(cmd, "setenv") == 0 || strcmp(cmd, "exit") == 0;
}


// Execute built-in commands
void execute_builtin_commands(char *arguments[]) {
    if (strcmp(arguments[0], "cd") == 0) {
        if (chdir(arguments[1]) != 0) perror("cd error");
    } else if (strcmp(arguments[0], "pwd") == 0) {
        char cwd[1024];
        if (getcwd(cwd, sizeof(cwd)) != NULL) printf("%s\n", cwd);
        else perror("pwd error");
    } else if (strcmp(arguments[0], "echo") == 0) {
        for (int i = 1; arguments[i] != NULL; i++) {
            if (arguments[i][0] == '$') {
                char *env_var = getenv(arguments[i] + 1);
                printf("%s ", env_var ? env_var : "");
            } else {
                printf("%s ", arguments[i]);
            }
        }
        printf("\n");
    } else if (strcmp(arguments[0], "env") == 0) {
        for (int i = 0; environ[i] != NULL; i++) printf("%s\n", environ[i]);
    } else if (strcmp(arguments[0], "setenv") == 0) {
        if (arguments[1] && arguments[2]) {
            setenv(arguments[1], arguments[2], 1);
        } else {
            printf("Usage: setenv <VAR> <VALUE>\n");
        }
    } else if (strcmp(arguments[0], "exit") == 0) {
        exit(0);
    }
}



// Handle output redirection with `>`
void handle_output_redirection(char *arguments[]) {
    for (int i = 0; arguments[i] != NULL; i++) {
        if (strcmp(arguments[i], ">") == 0) {
            int fd = open(arguments[i + 1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if (fd < 0) {
                perror("open error");
                exit(1);
            }
            dup2(fd, STDOUT_FILENO);
            close(fd);
            arguments[i] = NULL;
            break;
        }
    }
}

// Handle input redirection with `<`
void handle_input_redirection(char *arguments[]) {
    for (int i = 0; arguments[i] != NULL; i++) {
        if (strcmp(arguments[i], "<") == 0) {
            int fd = open(arguments[i + 1], O_RDONLY);
            if (fd < 0) {
                perror("open error");
                exit(1);
            }
            dup2(fd, STDIN_FILENO);
            close(fd);
            arguments[i] = NULL;
            break;
        }
    }
}
//df

// Handle piping with `|`
bool handle_pipe(char *arguments[]) {
    int i = 0;
    while (arguments[i] != NULL) {
        if (strcmp(arguments[i], "|") == 0) {
            arguments[i] = NULL;

            int pipefd[2];
            if (pipe(pipefd) == -1) {
                perror("pipe error");
                return false;
            }

            pid_t pid1 = fork();
            if (pid1 == 0) {
                dup2(pipefd[1], STDOUT_FILENO);  // Redirect stdout to pipe write end
                close(pipefd[0]);  // Close unused read end
                close(pipefd[1]);
                execvp(arguments[0], arguments);
                perror("execvp error");
                exit(1);
            } else if (pid1 < 0) {
                perror("fork error");
                return false;
            }

            pid_t pid2 = fork();
            if (pid2 == 0) {
                dup2(pipefd[0], STDIN_FILENO);  // Redirect stdin to pipe read end
                close(pipefd[1]);  // Close unused write end
                close(pipefd[0]);
                execvp(arguments[i + 1], &arguments[i + 1]);
                perror("execvp error");
                //df
                exit(1);
            } else if (pid2 < 0) {
                perror("fork error");
                return false;
            }

            close(pipefd[0]);
            close(pipefd[1]);
            waitpid(pid1, NULL, 0);  // Wait for the first process
            waitpid(pid2, NULL, 0);  // Wait for the second process
            return true;  // Pipe was handled
        }
        i++;
    }
    return false;  // No pipe found
}

// Execute external commands
void execute_command(char *arguments[], bool is_background) {
    if (handle_pipe(arguments)) return;  // Skip further execution if pipe handled

    pid_t pid = fork();
    if (pid == 0) {
        signal(SIGINT, SIG_DFL);  // Restore default SIGINT behavior in child
        handle_output_redirection(arguments);  // Handle output redirection
        handle_input_redirection(arguments);  // Handle input redirection
        execvp(arguments[0], arguments);  // Execute the command
        perror("execvp error");
        exit(1);
    } else if (pid < 0) {
        perror("fork error");
    } else {
        if (!is_background) {
            fg_process_pid = pid;  // Track the foreground process PID
            alarm(10);  // Set a 10-second timer for long-running processes
            waitpid(pid, NULL, 0);  // Wait for the child process to complete
            alarm(0);  // Cancel the timer
            fg_process_pid = -1;  // Reset the foreground process PID
        } else {
            printf("Process running in background with PID: %d\n", pid);
        }
    }
}



// Main function to run the shell
int main() {
    char command_line[MAX_COMMAND_LINE_LEN];
    char *arguments[MAX_COMMAND_LINE_ARGS];


    signal(SIGINT, handle_sigint);
    signal(SIGALRM, handle_sigalrm);

    while (true) {
        printf("%s", get_prompt());
        fflush(stdout);
        //d

        if (fgets(command_line, MAX_COMMAND_LINE_LEN, stdin) == NULL) break;

        command_line[strlen(command_line) - 1] = '\0';
        tokenize_command(command_line, arguments);

        bool is_background = (strcmp(arguments[0], "&") == 0);
        if (is_builtin_command(arguments[0])) {
            execute_builtin_commands(arguments);
        } else {
            execute_command(arguments, is_background);
        }
    }
    return 0;
}
