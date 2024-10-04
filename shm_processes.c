#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>

void ParentProcess(int *ShmPTR);
void ChildProcess(int *ShmPTR);

int main() {
    int ShmID;
    int *ShmPTR;
    pid_t pid;
    int status;

    // Create shared memory for two integers: BankAccount and Turn
    ShmID = shmget(IPC_PRIVATE, 2 * sizeof(int), IPC_CREAT | 0666);
    if (ShmID < 0) {
        printf("*** shmget error (server) ***\n");
        exit(1);
    }
    printf("Server has received a shared memory of two integers...\n");

    // Attach the shared memory
    ShmPTR = (int *) shmat(ShmID, NULL, 0);
    if ((long) ShmPTR == -1) {
        printf("*** shmat error (server) ***\n");
        exit(1);
    }
    printf("Server has attached the shared memory...\n");

    // Initialize BankAccount and Turn
    ShmPTR[0] = 0; // BankAccount
    ShmPTR[1] = 0; // Turn (0 = Parent's turn, 1 = Child's turn)

    printf("Server is about to fork a child process...\n");
    pid = fork();
    if (pid < 0) {
        printf("*** fork error (server) ***\n");
        exit(1);
    } else if (pid == 0) {
        // Child process
        ChildProcess(ShmPTR);
        exit(0);
    } else {
        // Parent process
        ParentProcess(ShmPTR);
        wait(&status);
        printf("Server has detected the completion of its child...\n");

        // Detach and remove shared memory
        shmdt((void *) ShmPTR);
        printf("Server has detached its shared memory...\n");
        shmctl(ShmID, IPC_RMID, NULL);
        printf("Server has removed its shared memory...\n");
        printf("Server exits...\n");
        exit(0);
    }
}

void ParentProcess(int *ShmPTR) {
    srand(time(0)); // Seed random number generator
    int *BankAccount = &ShmPTR[0];
    int *Turn = &ShmPTR[1];

    for (int i = 0; i < 25; i++) {
        sleep(rand() % 6); // Sleep 0-5 seconds

        int account = *BankAccount;
        while (*Turn != 0); // Wait for parent's turn

        if (account <= 100) {
            int deposit = rand() % 101; // Random deposit 0-100
            if (deposit % 2 == 0) { // Only deposit if even
                account += deposit;
                printf("Dear old Dad: Deposits $%d / Balance = $%d\n", deposit, account);
            } else {
                printf("Dear old Dad: Doesn't have any money to give\n");
            }
        } else {
            printf("Dear old Dad: Thinks Student has enough Cash ($%d)\n", account);
        }

        *BankAccount = account; // Update shared memory with new balance
        *Turn = 1; // Hand over turn to child
    }
}

void ChildProcess(int *ShmPTR) {
    srand(time(0) + 1); // Different seed for randomness
    int *BankAccount = &ShmPTR[0];
    int *Turn = &ShmPTR[1];

    for (int i = 0; i < 25; i++) {
        sleep(rand() % 6); // Sleep 0-5 seconds

        int account = *BankAccount;
        while (*Turn != 1); // Wait for child's turn

        int need = rand() % 51; // Random withdraw amount 0-50
        printf("Poor Student needs $%d\n", need);
        if (need <= account) { // Withdraw if balance is enough
            account -= need;
            printf("Poor Student: Withdraws $%d / Balance = $%d\n", need, account);
        } else {
            printf("Poor Student: Not Enough Cash ($%d)\n", account);
        }

        *BankAccount = account; // Update shared memory with new balance
        *Turn = 0; // Hand over turn to parent
    }
}
