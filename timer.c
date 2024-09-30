#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <time.h>

// Global variables for time tracking
volatile sig_atomic_t alarms = 0;
time_t start_time;

void alarm_handler(int signum) {
  // Signal handler for SIGALRM
  alarms++;  // Increment the alarm count
  printf("Hello World!\n");
  alarm(1);  // Set the alarm to trigger every second
}

void sigint_handler(int signum) {
  // Signal handler for SIGINT (Ctrl+C)
  time_t end_time = time(NULL);
  printf("\nProgram ran for %ld seconds, with %d alarms delivered.\n", end_time - start_time, alarms);
  exit(0);  // Exit the program
}

int main(int argc, char * argv[]) {
  start_time = time(NULL);  // Record the start time
  
  // Register signal handlers
  signal(SIGALRM, alarm_handler);  // Register handler for SIGALRM
  signal(SIGINT, sigint_handler);  // Register handler for SIGINT (Ctrl+C)
  
  alarm(1);  // Schedule SIGALRM every 1 second
  
  // Busy wait for signals
  while (1) {
    pause();  // Wait for signals
  }

  return 0;
}

