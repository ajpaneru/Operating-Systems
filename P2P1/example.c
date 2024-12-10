#include <semaphore.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>

int main(int argc, char **argv)
{
    int fd, i, nloop = 10, zero = 0, *counter_ptr;
    sem_t *mutex;

    // Open a file and map it into memory to hold the shared counter
    fd = open("log.txt", O_RDWR | O_CREAT, S_IRWXU);
    write(fd, &zero, sizeof(int));
    counter_ptr = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    close(fd);

    // Create and initialize semaphore
    if ((mutex = sem_open("examplesemaphore", O_CREAT, 0644, 1)) == SEM_FAILED) {
        perror("Semaphore initialization failed");
        exit(1);
    }

    // Create child process
    if (fork() == 0) {
        // Child process execution
        for (i = 0; i < nloop; i++) {
            printf("Child wanting to enter critical section\n");
            sem_wait(mutex);  // Enter critical section
            printf("Child entered critical section: %d\n", (*counter_ptr)++);
            sleep(2);  // Simulate processing
            printf("Child leaving critical section\n");
            sem_post(mutex);  // Exit critical section
            sleep(1);  // Simulate waiting
        }
        exit(0);  // Exit child process
    }

    // Back to parent process execution
    for (i = 0; i < nloop; i++) {
        printf("Parent wanting to enter critical section\n");
        sem_wait(mutex);  // Enter critical section
        printf("Parent entered critical section: %d\n", (*counter_ptr)++);
        sleep(2);  // Simulate processing
        printf("Parent leaving critical section\n");
        sem_post(mutex);  // Exit critical section
        sleep(1);  // Simulate waiting
    }

    exit(0);  // Exit parent process
}
