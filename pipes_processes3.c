#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>

/**
 * Executes the command "cat scores | grep <argument> | sort".
 * The argument is passed from the command line.
 * Example: ./pipes_processes3 28
 */

int main(int argc, char **argv)
{
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <search_term>\n", argv[0]);
        exit(1);
    }

    int pipefd1[2];  // Pipe between cat and grep
    int pipefd2[2];  // Pipe between grep and sort
    int pid1, pid2;

    char *cat_args[] = {"cat", "scores", NULL};
    char *grep_args[] = {"grep", argv[1], NULL};
    char *sort_args[] = {"sort", NULL};

    // Create first pipe (cat -> grep)
    pipe(pipefd1);

    // Fork first child for grep
    if ((pid1 = fork()) == 0)
    {
        // Inside first child process (grep)

        // Create second pipe (grep -> sort)
        pipe(pipefd2);

        // Fork second child for sort
        if ((pid2 = fork()) == 0)
        {
            // Inside second child process (sort)

            // Replace standard input with input part of the second pipe
            dup2(pipefd2[0], 0);

            // Close unused pipe ends
            close(pipefd2[1]);  // Close the write end of the second pipe
            close(pipefd1[0]);  // Close read end of the first pipe (not needed in this process)
            close(pipefd1[1]);  // Close write end of the first pipe

            // Execute sort
            execvp("sort", sort_args);
        }
        else
        {
            // Inside first child process (grep)

            // Replace standard input with input part of the first pipe
            dup2(pipefd1[0], 0);

            // Replace standard output with output part of the second pipe
            dup2(pipefd2[1], 1);

            // Close unused pipe ends
            close(pipefd1[1]);  // Close the write end of the first pipe
            close(pipefd2[0]);  // Close the read end of the second pipe

            // Execute grep
            execvp("grep", grep_args);
        }
    }
    else
    {
        // Inside parent process (cat)

        // Replace standard output with output part of the first pipe
        dup2(pipefd1[1], 1);

        // Close unused pipe ends
        close(pipefd1[0]);  // Close the read end of the first pipe

        // Execute cat
        execvp("cat", cat_args);

        // Wait for child processes to finish
        wait(NULL);
    }

    return 0;
}



