#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>

#define MAX_ITER 30 // Maximum number of iterations for each child

void ChildProcess();  // Child process prototype
void ParentProcess(pid_t, pid_t);  // Parent process prototype

int main(void) {
    pid_t pid1, pid2;

    // Seed the random number generator
    srand(time(NULL));

    // Create the first child process
    pid1 = fork();
    if (pid1 == 0) {
        ChildProcess();  // First child process
    } else {
        // Create the second child process
        pid2 = fork();
        if (pid2 == 0) {
            ChildProcess();  // Second child process
        } else {
            ParentProcess(pid1, pid2);  // Parent process waits for both children
        }
    }

    return 0;
}

void ChildProcess() {
    int iterations = rand() % MAX_ITER + 1;  // Random number of iterations (1 to 30)
    int i;  // Declare variable outside the loop
    for (i = 0; i < iterations; i++) {
        printf("Child Pid: %d is going to sleep!\n", getpid());
        int sleep_time = rand() % 10;  // Sleep for random time (0 to 9 seconds)
        sleep(sleep_time);  // Child sleeps
        printf("Child Pid: %d is awake! Where is my Parent: %d?\n", getpid(), getppid());
    }
    printf("Child Pid: %d has finished its work and is exiting.\n", getpid());
    exit(0);  // Terminate the child process
}

void ParentProcess(pid_t pid1, pid_t pid2) {
    int status;

    // Wait for the first child to complete
    waitpid(pid1, &status, 0);
    printf("Child Pid: %d has completed\n", pid1);

    // Wait for the second child to complete
    waitpid(pid2, &status, 0);
    printf("Child Pid: %d has completed\n", pid2);

    printf("Parent process is done.\n");
}
