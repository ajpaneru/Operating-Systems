// Three-Way Pipe Implementation: "cat scores | grep <argument> | sort"
#include <stdio.h>        // For standard I/O functions
#include <unistd.h>       // For pipe(), fork(), dup2(), execvp()
#include <fcntl.h>        // For file control options
#include <sys/types.h>    // For system data types
#include <sys/stat.h>     // For file status functions
#include <stdlib.h>       // For exit()

int main(int argc, char **argv)
{
    // Validate command-line argument
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <grep-argument>\n", argv[0]);
        exit(1);
    }

    int pipefd1[2];  // Pipe for communication between cat and grep
    int pipefd2[2];  // Pipe for communication between grep and sort
    int pid1, pid2;  // Process IDs for children

    // Create the first pipe
    pipe(pipefd1);

    // Fork the first child process for "grep <argument>"
    pid1 = fork();

    if (pid1 == 0)  // First child process
    {
        // Create the second pipe
        pipe(pipefd2);

        // Fork the second child process for "sort"
        pid2 = fork();
        
        if (pid2 == 0)  // Second child process
        {
            // Redirect standard input to read-end of pipe 2
            dup2(pipefd2[0], STDIN_FILENO);

            // Close all unused pipe ends
            close(pipefd2[1]);
            close(pipefd1[0]);
            close(pipefd1[1]);

            // Execute "sort"
            char *sort_args[] = {"sort", NULL};
            execvp("sort", sort_args);
        }
        else  // First child process continues
        {
            // Redirect standard input to read-end of pipe 1
            dup2(pipefd1[0], STDIN_FILENO);

            // Redirect standard output to write-end of pipe 2
            dup2(pipefd2[1], STDOUT_FILENO);

            // Close all unused pipe ends
            close(pipefd1[1]);
            close(pipefd2[0]);
            close(pipefd2[1]);

            // Execute "grep <argument>"
            char *grep_args[] = {"grep", argv[1], NULL};
            execvp("grep", grep_args);
        }
    }
    else  // Parent process
    {
        // Redirect standard output to write-end of pipe 1
        dup2(pipefd1[1], STDOUT_FILENO);

        // Close all unused pipe ends
        close(pipefd1[0]);
        close(pipefd1[1]);

        // Execute "cat scores"
        char *cat_args[] = {"cat", "scores", NULL};
        execvp("cat", cat_args);
    }

    return 0;  // This point should never be reached
}
