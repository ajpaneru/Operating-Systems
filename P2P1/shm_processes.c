#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>

void ClientProcess(int []);

int main(int argc, char *argv[])
{
    int ShmID;
    int *ShmPTR;
    pid_t pid;
    int status;

    // Check for correct number of arguments
    if (argc != 5) {
        printf("Usage: %s #1 #2 #3 #4\n", argv[0]);
        exit(1);
    }

    // Create shared memory
    ShmID = shmget(IPC_PRIVATE, 4 * sizeof(int), IPC_CREAT | 0666);
    if (ShmID < 0) {
        printf("*** shmget error (server) ***\n");
        exit(1);
    }
    printf("Server has received a shared memory of four integers...\n");

    // Attach shared memory
    ShmPTR = (int *)shmat(ShmID, NULL, 0);
    if (*ShmPTR == -1) {
        printf("*** shmat error (server) ***\n");
        exit(1);
    }
    printf("Server has attached the shared memory...\n");

    // Fill shared memory with input values
    ShmPTR[0] = atoi(argv[1]);
    ShmPTR[1] = atoi(argv[2]);
    ShmPTR[2] = atoi(argv[3]);
    ShmPTR[3] = atoi(argv[4]);
    printf("Server has filled %d %d %d %d in shared memory...\n",
           ShmPTR[0], ShmPTR[1], ShmPTR[2], ShmPTR[3]);

    // Fork a child process
    printf("Server is about to fork a child process...\n");
    pid = fork();
    if (pid < 0) {
        printf("*** fork error (server) ***\n");
        exit(1);
    }
    else if (pid == 0) {
        // Child process execution
        ClientProcess(ShmPTR);
        exit(0);
    }

    // Wait for child process to complete
    wait(&status);
    printf("Server has detected the completion of its child...\n");

    // Detach shared memory
    shmdt((void *)ShmPTR);
    printf("Server has detached its shared memory...\n");

    // Remove shared memory
    shmctl(ShmID, IPC_RMID, NULL);
    printf("Server has removed its shared memory...\n");
    printf("Server exits...\n");

    exit(0);
}

// Function executed by client process
void ClientProcess(int SharedMem[])
{
    printf("   Client process started\n");
    printf("   Client found %d %d %d %d in shared memory\n",
           SharedMem[0], SharedMem[1], SharedMem[2], SharedMem[3]);
    printf("   Client is about to exit\n");
}
