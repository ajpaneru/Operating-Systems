#include <stdio.h>
#include <limits.h>
#include <stdlib.h>
#include "process.h"
#include "util.h"

// Function declarations for various scheduling algorithms
void findWaitingTime(ProcessType plist[], int n);
void findTurnAroundTime(ProcessType plist[], int n);

/* Round Robin: Calculates waiting time */
void findWaitingTimeRR(ProcessType plist[], int n, int quantum) {
    int rem_bt[n];  // Remaining burst time

    // Initialize remaining burst times from original burst times
    for (int i = 0; i < n; i++) {
        rem_bt[i] = plist[i].bt;
    }

    int t = 0;  // Tracks current time

    while (1) {
        int finished = 1;  // Assume all processes are done

        for (int i = 0; i < n; i++) {
            if (rem_bt[i] > 0) {
                finished = 0;  // There is at least one process left

                if (rem_bt[i] > quantum) {
                    t += quantum;         // Increase current time
                    rem_bt[i] -= quantum; // Process is partially executed
                } else {
                    t += rem_bt[i];                // Process completes
                    plist[i].wt = t - plist[i].bt; // Calculate waiting time
                    rem_bt[i] = 0;                // Mark process as done
                }
            }
        }

        // If all processes are finished, exit the loop
        if (finished) break;
    }
}

/* SJF: Calculates waiting time based on shortest job next */
void findWaitingTimeSJF(ProcessType plist[], int n) {
    int rt[n];  // Remaining time for processes

    // Initialize remaining times with burst times
    for (int i = 0; i < n; i++) {
        rt[i] = plist[i].bt;
    }

    int completed = 0, t = 0, minm = INT_MAX;
    int shortest = 0, finish_time;
    int found = 0;  // Flag to check if process is found

    // Process scheduling
    while (completed != n) {
        for (int j = 0; j < n; j++) {
            if ((plist[j].art <= t) && (rt[j] < minm) && rt[j] > 0) {
                minm = rt[j];
                shortest = j;
                found = 1;
            }
        }

        // No process found, move to the next time unit
        if (!found) {
            t++;
            continue;
        }

        rt[shortest]--;  // Execute the shortest process by 1 unit
        minm = rt[shortest] ? rt[shortest] : INT_MAX;

        // Process completed
        if (rt[shortest] == 0) {
            completed++;
            finish_time = t + 1;
            plist[shortest].wt = finish_time - plist[shortest].bt - plist[shortest].art;

            // Correct negative waiting time
            if (plist[shortest].wt < 0) plist[shortest].wt = 0;
        }
        t++;  // Increment time
    }
}

/* FCFS: Simple calculation of waiting time */
void findWaitingTime(ProcessType plist[], int n) {
    plist[0].wt = 0;  // First process has zero waiting time

    // Calculate waiting time for all remaining processes
    for (int i = 1; i < n; i++) {
        plist[i].wt = plist[i - 1].bt + plist[i - 1].wt;
    }
}

/* Calculates turnaround time for processes */
void findTurnAroundTime(ProcessType plist[], int n) {
    for (int i = 0; i < n; i++) {
        plist[i].tat = plist[i].bt + plist[i].wt;
    }
}

/* Custom comparison function for Priority Scheduling */
int comparePriority(const void *a, const void *b) {
    ProcessType *p1 = (ProcessType *)a;
    ProcessType *p2 = (ProcessType *)b;

    return (p1->pri - p2->pri);  // Sort in ascending order of priority
}

/* Priority Scheduling: Average time calculation */
void findavgTimePriority(ProcessType plist[], int n) {
    qsort(plist, n, sizeof(ProcessType), comparePriority);  // Sort by priority
    findWaitingTime(plist, n);  // Calculate waiting time
    findTurnAroundTime(plist, n);  // Calculate turnaround time
    printf("\nPriority Scheduling Results\n");
}

/* FCFS: Average time calculation */
void findavgTimeFCFS(ProcessType plist[], int n) {
    findWaitingTime(plist, n);  // Calculate waiting time
    findTurnAroundTime(plist, n);  // Calculate turnaround time
    printf("\nFCFS Scheduling Results\n");
}

/* SJF: Average time calculation */
void findavgTimeSJF(ProcessType plist[], int n) {
    findWaitingTimeSJF(plist, n);  // Calculate waiting time
    findTurnAroundTime(plist, n);  // Calculate turnaround time
    printf("\nSJF Scheduling Results\n");
}

/* Round Robin: Average time calculation */
void findavgTimeRR(ProcessType plist[], int n, int quantum) {
    findWaitingTimeRR(plist, n, quantum);  // Calculate waiting time
    findTurnAroundTime(plist, n);  // Calculate turnaround time
    printf("\nRound Robin Scheduling (Quantum = %d)\n", quantum);
}

/* Prints the results in a formatted table */
void printMetrics(ProcessType plist[], int n) {
    int total_wt = 0, total_tat = 0;  // Sums for averages

    printf("\nProcess\tBurst Time\tWaiting Time\tTurnaround Time\n");

    for (int i = 0; i < n; i++) {
        total_wt += plist[i].wt;
        total_tat += plist[i].tat;
        printf("%d\t\t%d\t\t%d\t\t%d\n", plist[i].pid, plist[i].bt, plist[i].wt, plist[i].tat);
    }

    printf("\nAverage Waiting Time = %.2f", (float)total_wt / n);
    printf("\nAverage Turnaround Time = %.2f\n", (float)total_tat / n);
}

/* Initializes the process list from file */
ProcessType *initProc(char *filename, int *n) {
    FILE *input_file = fopen(filename, "r");
    if (!input_file) {
        fprintf(stderr, "Error: Could not open file\n");
        exit(1);
    }

    ProcessType *plist = parse_file(input_file, n);
    fclose(input_file);
    return plist;
}

/* Main function: Simulates different scheduling algorithms */
int main(int argc, char *argv[]) {
    int n, quantum = 2;  // Default quantum for Round Robin
    ProcessType *proc_list;

    if (argc < 2) {
        fprintf(stderr, "Usage: ./schedsim <input-file-path>\n");
        return 1;
    }

    // Simulate FCFS
    proc_list = initProc(argv[1], &n);
    findavgTimeFCFS(proc_list, n);
    printMetrics(proc_list, n);

    // Simulate SJF
    proc_list = initProc(argv[1], &n);
    findavgTimeSJF(proc_list, n);
    printMetrics(proc_list, n);

    // Simulate Priority Scheduling
    proc_list = initProc(argv[1], &n);
    findavgTimePriority(proc_list, n);
    printMetrics(proc_list, n);

    // Simulate Round Robin
    proc_list = initProc(argv[1], &n);
    findavgTimeRR(proc_list, n, quantum);
    printMetrics(proc_list, n);

    return 0;  // End of program
}
