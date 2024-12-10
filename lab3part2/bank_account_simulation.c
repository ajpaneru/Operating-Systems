#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <time.h>

#define SHM_KEY 9876 // Key used to identify shared memory

// Function declarations for parent and child processes
void ParentProcess(int *BankAccount, int *Turn); // Parent function (Dear Old Dad)
void ChildProcess(int *BankAccount, int *Turn);  // Child function (Poor Student)

int main() {
    int shm_id;
    int *ShmPTR;
    pid_t pid;
    int status;

    // Create a shared memory segment for storing two integers
    shm_id = shmget(SHM_KEY, 2 * sizeof(int), IPC_CREAT | 0666);
    if (shm_id < 0) {
        printf("*** Error creating shared memory ***\n");
        exit(1);
    }
    printf("Shared memory successfully created.\n");

    // Attach shared memory to the process
    ShmPTR = (int *) shmat(shm_id, NULL, 0);
    if (ShmPTR == (int *) -1) {
        printf("*** Error attaching shared memory ***\n");
        exit(1);
    }
    printf("Shared memory successfully attached.\n");

    // Initialize shared memory values
    ShmPTR[0] = 0; // Initialize BankAccount balance to 0
    ShmPTR[1] = 0; // Parent goes first (Turn = 0)

    // Create a child process
    pid = fork();
    if (pid < 0) {
        printf("*** Error creating child process ***\n");
        exit(1);
    }

    if (pid == 0) {
        // Child process execution
        ChildProcess(&ShmPTR[0], &ShmPTR[1]);
        exit(0);
    } else {
        // Parent process execution
        ParentProcess(&ShmPTR[0], &ShmPTR[1]);

        // Wait for the child process to finish
        wait(&status);

        // Detach and delete shared memory
        shmdt(ShmPTR);
        shmctl(shm_id, IPC_RMID, NULL);
        printf("Shared memory successfully detached and deleted.\n");
    }

    return 0;
}

// Parent process function simulating deposits to the bank account
void ParentProcess(int *BankAccount, int *Turn) {
    int account, deposit;
    srand(time(NULL)); // Initialize random generator for parent

    for (int i = 0; i < 25; i++) {
        sleep(rand() % 6); // Random delay between 0 and 5 seconds

        account = *BankAccount;
        while (*Turn != 0); // Wait until it's the parent's turn

        // Deposit money if the account has $100 or less
        if (account <= 100) {
            deposit = rand() % 101; // Generate deposit amount between 0 and 100

            if (deposit % 2 == 0) { // Deposit money if even
                account += deposit;
                printf("Dear Old Dad: Deposits $%d / New Balance = $%d\n", deposit, account);
            } else {
                printf("Dear Old Dad: No money to give right now\n");
            }
        } else {
            printf("Dear Old Dad: Thinks student has enough money ($%d)\n", account);
        }

        // Update shared memory and pass the turn to the child
        *BankAccount = account;
        *Turn = 1; 
    }
}

// Child process function simulating withdrawals from the bank account
void ChildProcess(int *BankAccount, int *Turn) {
    int account, withdrawal;
    srand(time(NULL) + 1); // Different random seed for child

    for (int i = 0; i < 25; i++) {
        sleep(rand() % 6); // Random delay between 0 and 5 seconds

        account = *BankAccount;
        while (*Turn != 1); // Wait until it's the child's turn

        // Request withdrawal of a random amount between 0 and 50
        withdrawal = rand() % 51;
        printf("Poor Student: Requests $%d\n", withdrawal);

        // Check if enough funds are available for withdrawal
        if (withdrawal <= account) {
            account -= withdrawal;
            printf("Poor Student: Withdraws $%d / Remaining Balance = $%d\n", withdrawal, account);
        } else {
            printf("Poor Student: Insufficient funds ($%d)\n", account);
        }

        // Update shared memory and pass the turn back to the parent
        *BankAccount = account;
        *Turn = 0; 
    }
}

