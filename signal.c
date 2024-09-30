#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

void handler(int signum) {
  // Signal handler for SIGALRM
  printf("Hello World!\n");
  alarm(5); // Re-arm the alarm for another 5 seconds
}

int main(int argc, char * argv[]) {
  signal(SIGALRM, handler); // Register handler to handle SIGALRM
  alarm(5); // Schedule a SIGALRM for 5 seconds
  
  // Loop indefinitely, printing "Turing was right!" after each alarm
  while (1) {
    pause(); // Wait for signal (SIGALRM)
    printf("Turing was right!\n");
  }

  return 0;
}
