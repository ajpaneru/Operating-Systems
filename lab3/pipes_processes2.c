// Demonstrates piping between two commands "cat scores | grep Lakers"
#include <stdio.h>        // For standard I/O functions
#include <unistd.h>       // For pipe(), fork(), dup2(), execvp()
#include <fcntl.h>        // For file control options
#include <sys/types.h>    // For system data types
#include <sys/stat.h>     // For file mode constants

int main(int argc, char **argv)
{
    int pipefd[2];   // Array to store pipe file descriptors
    int pid;         // Process ID for fork()

    // Define arguments for "cat scores" and "grep Lakers"
    char *cat_args[] = {"cat", "scores", NULL};
    char *grep_args[] = {"grep", "Lakers", NULL};

    // Create a pipe, pipe file descriptors go into pipefd[0] and pipefd[1]
    pipe(pipefd);

    // Create a child process
    pid = fork();

    if (pid == 0)  // Child process
    {
        // Handle "grep Lakers"

        // Redirect standard input to read end of pipe
        dup2(pipefd[0], STDIN_FILENO);

        // Close unused write end of the pipe
        close(pipefd[1]);

        // Execute "grep Lakers"
        execvp("grep", grep_args);
    }
    else  // Parent process
    {
        // Handle "cat scores"

        // Redirect standard output to write end of pipe
        dup2(pipefd[1], STDOUT_FILENO);

        // Close unused read end of the pipe
        close(pipefd[0]);

        // Execute "cat scores"
        execvp("cat", cat_args);
    }

    return 0;  // This will never be reached
}
