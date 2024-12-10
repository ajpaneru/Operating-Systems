#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<errno.h>

#include "process.h"
#include "util.h"

#define DEBUG 0    // Set to 1 to enable detailed debug output

// Function pointer type for comparing two elements
typedef int (*Comparer)(const void *a, const void *b);

// Comparison function for processes based on specific rules
int my_comparer(const void *this, const void *that)
{
      Process *p1 = (Process *)this;
      Process *p2 = (Process *)that;

      // Compare by priority in descending order
      if (p1->priority != p2->priority) 
      {
            return p2->priority - p1->priority;
      }
      
      // If priority is equal, compare by arrival time in ascending order
      if (p1->arrival_time != p2->arrival_time) 
      {
            return p1->arrival_time - p2->arrival_time;
      }

      // If both priority and arrival time are the same, compare by PID in ascending order
      return p1->pid - p2->pid;
}

int main(int argc, char *argv[])
{
      if (argc < 2) 
      {
            fprintf(stderr, "Usage: ./func-ptr <input-file-path>\n");
            fflush(stdout);
            return 1;
      }

      // Open the input file and check for errors
      FILE *input_file = fopen(argv[1], "r");
      if (!input_file) 
      {
            fprintf(stderr, "Error: Could not open the file.\n");
            fflush(stdout);
            return 1;
      }

      // Parse the file and load process data
      Process *processes = parse_file(input_file);

      // Assign comparison function to a function pointer
      Comparer process_comparer = &my_comparer;

#if DEBUG
      // Print the original list if debugging is enabled
      for (int i = 0; i < P_SIZE; i++) 
      {
            printf("%d (%d, %d) ", processes[i].pid, processes[i].priority, processes[i].arrival_time);
      }
      printf("\n");
#endif

      // Sort processes using the custom comparison function
      qsort(processes, P_SIZE, sizeof(Process), process_comparer);

      // Print the sorted list of processes
      for (int i = 0; i < P_SIZE; i++) 
      {
            printf("%d (%d, %d)\n", processes[i].pid, processes[i].priority, processes[i].arrival_time);
      }
      
      fflush(stdout);
      fflush(stderr);

      // Clean up allocated memory and close the file
      free(processes);
      fclose(input_file);

      return 0;
}
