#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <semaphore.h>
#include <time.h>
#include <errno.h>

#define MAX_CHILDREN 10

// Function declarations
void dear_old_dad(int *bank_account, sem_t *sem_mutex);
void poor_student(int *bank_account, sem_t *sem_mutex, int id);
void lovable_mom(int *bank_account, sem_t *sem_mutex);

int main(int argc, char *argv[]) {
    // Check command-line arguments
    if (argc < 3) {
        printf("Usage: %s <number of parents> <number of children>\n", argv[0]);
        exit(1);
    }

    int num_parents = atoi(argv[1]);
    int num_children = atoi(argv[2]);

    // Validate maximum number of children
    if (num_children > MAX_CHILDREN) {
        printf("Maximum number of children allowed: %d\n", MAX_CHILDREN);
        exit(1);
    }

    // Create shared memory for the bank account
    int fd = open("bank_account.txt", O_RDWR | O_CREAT, S_IRWXU);
    if (fd == -1) {
        perror("Failed to open shared memory file");
        exit(1);
    }

    int zero = 0;
    write(fd, &zero, sizeof(int));
    int *bank_account = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (bank_account == MAP_FAILED) {
        perror("Failed to map shared memory");
        exit(1);
    }
    close(fd);

    *bank_account = 0;  // Initialize bank account balance

    // Create semaphore for synchronization
    sem_t *sem_mutex = sem_open("bank_semaphore", O_CREAT, 0644, 1);
    if (sem_mutex == SEM_FAILED) {
        perror("Failed to initialize semaphore");
        exit(1);
    }

    // Fork processes for parents and children
    pid_t pid;
    int i;
    for (i = 0; i < num_parents + num_children; i++) {
        pid = fork();
        if (pid == 0) {  // Child process
            srand(time(NULL) + getpid());
            if (i < num_parents) {
                if (i == 0)
                    dear_old_dad(bank_account, sem_mutex);  // First parent acts as Dad
                else
                    lovable_mom(bank_account, sem_mutex);  // Remaining parents act as Mom
            } else {
                poor_student(bank_account, sem_mutex, i - num_parents);  // Children processes
            }
            exit(0);
        } else if (pid < 0) {  // Fork error handling
            perror("Fork failed");
            exit(1);
        }
    }

    // Wait for all child processes to complete
    for (i = 0; i < num_parents + num_children; i++) {
        wait(NULL);
    }

    // Clean up resources
    munmap(bank_account, sizeof(int));
    sem_close(sem_mutex);
    sem_unlink("bank_semaphore");
    unlink("bank_account.txt");

    return 0;
}

// Function for Dad's bank account interactions
void dear_old_dad(int *bank_account, sem_t *sem_mutex) {
    while (1) {
        sleep(rand() % 5);  // Simulate waiting time
        printf("Dear Old Dad: Attempting to Check Balance\n");
        sem_wait(sem_mutex);  // Enter critical section

        int local_balance = *bank_account;
        if (rand() % 2 == 0) {  // Random decision making
            if (local_balance < 100) {
                int deposit = (rand() % 101) + 1;  // Deposit between 1 and 100
                printf("Dear Old Dad: Deposits $%d / Balance = $%d\n", deposit, local_balance + deposit);
                *bank_account += deposit;
            } else {
                printf("Dear Old Dad: Thinks Student has enough Cash ($%d)\n", local_balance);
            }
        } else {
            printf("Dear Old Dad: Last Checking Balance = $%d\n", local_balance);
        }

        sem_post(sem_mutex);  // Exit critical section
    }
}

// Function for Student's bank account interactions
void poor_student(int *bank_account, sem_t *sem_mutex, int id) {
    while (1) {
        sleep(rand() % 5);  // Simulate waiting time
        printf("Poor Student %d: Attempting to Check Balance\n", id);
        sem_wait(sem_mutex);  // Enter critical section

        int local_balance = *bank_account;
        if (rand() % 2 == 0) {  // Random decision making
            int need = (rand() % 50) + 1;  // Need between 1 and 50
            printf("Poor Student %d needs $%d\n", id, need);
            if (local_balance >= need) {
                printf("Poor Student %d: Withdraws $%d / Balance = $%d\n", id, need, local_balance - need);
                *bank_account -= need;
            } else {
                printf("Poor Student %d: Not Enough Cash ($%d)\n", id, local_balance);
            }
        } else {
            printf("Poor Student %d: Last Checking Balance = $%d\n", id, local_balance);
        }

        sem_post(sem_mutex);  // Exit critical section
    }
}

// Function for Mom's bank account interactions
void lovable_mom(int *bank_account, sem_t *sem_mutex) {
    while (1) {
        sleep(rand() % 10);  // Simulate waiting time
        printf("Lovable Mom: Attempting to Check Balance\n");
        sem_wait(sem_mutex);  // Enter critical section

        int local_balance = *bank_account;
        if (local_balance <= 100) {
            int deposit = (rand() % 125) + 1;  // Deposit between 1 and 125
            printf("Lovable Mom: Deposits $%d / Balance = $%d\n", deposit, local_balance + deposit);
            *bank_account += deposit;
        }

        sem_post(sem_mutex);  // Exit critical section
    }
}

