// Shared Memory Demonstration with Fork and Client Process
#include <stdio.h>        // For standard I/O functions
#include <stdlib.h>       // For memory management and exit()
#include <sys/types.h>    // For data types
#include <sys/ipc.h>      // For shared memory keys
#include <sys/shm.h>      // For shared memory functions
#include <unistd.h>       // For fork() and process control
#include <sys/wait.h>     // For wait()

// Client Process Function Declaration
void ClientProcess(int []);

int main(int argc, char *argv[])
{
    int ShmID;          // Shared memory ID
    int *ShmPTR;        // Pointer to shared memory
    pid_t pid;          // Process ID for fork
    int status;         // Child process exit status

    // Validate command-line arguments
    if (argc != 5) {
        printf("Usage: %s #1 #2 #3 #4\n", argv[0]);
        exit(1);
    }

    // Create shared memory segment
    ShmID = shmget(IPC_PRIVATE, 4 * sizeof(int), IPC_CREAT | 0666);
    if (ShmID < 0) {
        printf("*** Error: shmget failed (server) ***\n");
        exit(1);
    }
    printf("Server successfully allocated shared memory of four integers...\n");

    // Attach shared memory to server's address space
    ShmPTR = (int *)shmat(ShmID, NULL, 0);
    if (*ShmPTR == -1) {
        printf("*** Error: shmat failed (server) ***\n");
        exit(1);
    }
    printf("Server successfully attached shared memory...\n");

    // Store input values into shared memory
    ShmPTR[0] = atoi(argv[1]);
    ShmPTR[1] = atoi(argv[2]);
    ShmPTR[2] = atoi(argv[3]);
    ShmPTR[3] = atoi(argv[4]);
    printf("Server stored values %d %d %d %d into shared memory...\n",
           ShmPTR[0], ShmPTR[1], ShmPTR[2], ShmPTR[3]);

    // Fork a child process
    printf("Server is forking a child process...\n");
    pid = fork();

    if (pid < 0) {
        printf("*** Error: fork failed (server) ***\n");
        exit(1);
    } else if (pid == 0) {
        // Child process execution
        ClientProcess(ShmPTR);
        exit(0);
    }

    // Wait for child process to complete
    wait(&status);
    printf("Server detected completion of its child process...\n");

    // Detach and remove shared memory
    shmdt((void *)ShmPTR);
    printf("Server detached shared memory...\n");

    shmctl(ShmID, IPC_RMID, NULL);
    printf("Server removed shared memory segment...\n");

    printf("Server exiting...\n");
    exit(0);
}

// Client process function
void ClientProcess(int SharedMem[])
{
    printf("   Client process started\n");
    printf("   Client read values %d %d %d %d from shared memory\n",
           SharedMem[0], SharedMem[1], SharedMem[2], SharedMem[3]);
    printf("   Client process is exiting\n");
}
