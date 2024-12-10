#include <limits.h>   // For defining INT_MAX
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "list.h"
#include "util.h"

// Convert a string to uppercase
void TOUPPER(char * arr) 
{
      for (int i = 0; i < strlen(arr); i++) 
      {
            arr[i] = toupper(arr[i]);
      }
}

// Parse input file and determine memory allocation policy
void get_input(char *args[], int input[][2], int *n, int *size, int *policy) 
{
      FILE *input_file = fopen(args[1], "r");
      
      if (!input_file) 
      {
            fprintf(stderr, "Error: Invalid file path\n");
            fflush(stdout);
            exit(0);
      }

      parse_file(input_file, input, n, size);
      fclose(input_file);

      TOUPPER(args[2]);

      if ((strcmp(args[2], "-F") == 0) || (strcmp(args[2], "-FIFO") == 0))
            *policy = 1;
      else if ((strcmp(args[2], "-B") == 0) || (strcmp(args[2], "-BESTFIT") == 0))
            *policy = 2;
      else if ((strcmp(args[2], "-W") == 0) || (strcmp(args[2], "-WORSTFIT") == 0))
            *policy = 3;
      else 
      {
            printf("usage: ./mmu <input file> -{F | B | W }\n(F=FIFO | B=BESTFIT | W=WORSTFIT)\n");
            exit(1);
      }
}

// Allocate memory based on the chosen policy
void allocate_memory(list_t *freelist, list_t *alloclist, int pid, int blocksize, int policy) 
{
      node_t *current = freelist->head;
      node_t *selected = NULL;
      block_t *blk = NULL;

      if (policy == 1)  // First Fit (FIFO)
      {
            while (current != NULL) 
            {
                  int size = current->blk->end - current->blk->start + 1;
                  if (size >= blocksize) 
                  {
                        selected = current;
                        break;
                  }
                  current = current->next;
            }
      }
      else if (policy == 2)  // Best Fit
      {
            int min_size = INT_MAX;
            while (current != NULL) 
            {
                  int size = current->blk->end - current->blk->start + 1;
                  if (size >= blocksize && size < min_size) 
                  {
                        min_size = size;
                        selected = current;
                  }
                  current = current->next;
            }
      }
      else if (policy == 3)  // Worst Fit
      {
            int max_size = -1;
            while (current != NULL) 
            {
                  int size = current->blk->end - current->blk->start + 1;
                  if (size >= blocksize && size > max_size) 
                  {
                        max_size = size;
                        selected = current;
                  }
                  current = current->next;
            }
      }

      if (selected == NULL) 
      {
            printf("Error: Not Enough Memory\n");
            return;
      }

      blk = selected->blk;
      int original_end = blk->end;

      blk->pid = pid;
      blk->end = blk->start + blocksize - 1;
      list_add_ascending_by_address(alloclist, blk);

      if (blk->end < original_end) 
      {
            block_t *fragment = malloc(sizeof(block_t));
            fragment->pid = 0;
            fragment->start = blk->end + 1;
            fragment->end = original_end;

            if (policy == 1)
                  list_add_to_back(freelist, fragment);
            else if (policy == 2)
                  list_add_ascending_by_blocksize(freelist, fragment);
            else if (policy == 3)
                  list_add_descending_by_blocksize(freelist, fragment);
      }

      list_remove_from_front(freelist);
}

// Deallocate memory for a process
void deallocate_memory(list_t *alloclist, list_t *freelist, int pid, int policy) 
{
      node_t *current = alloclist->head;
      node_t *previous = NULL;
      block_t *blk = NULL;

      while (current != NULL) 
      {
            if (current->blk->pid == pid) 
            {
                  blk = current->blk;

                  if (previous == NULL)
                        alloclist->head = current->next;
                  else
                        previous->next = current->next;

                  free(current);
                  break;
            }
            previous = current;
            current = current->next;
      }

      if (blk == NULL) 
      {
            printf("Error: Can't locate memory used by PID: %d\n", pid);
            return;
      }

      blk->pid = 0;
      list_add_ascending_by_address(freelist, blk);
}

// Merge adjacent free memory blocks
list_t* coalese_memory(list_t *list) 
{
      list_t *temp_list = list_alloc();
      block_t *blk;

      while ((blk = list_remove_from_front(list)) != NULL) 
      {
            list_add_ascending_by_address(temp_list, blk);
      }

      list_coalese_nodes(temp_list);
      return temp_list;
}

// Print memory allocation and free lists
void print_list(list_t *list, char *message) 
{
      node_t *current = list->head;
      block_t *blk;
      int i = 0;

      printf("%s:\n", message);

      while (current != NULL) 
      {
            blk = current->blk;
            printf("Block %d:\t START: %d\t END: %d", i, blk->start, blk->end);

            if (blk->pid != 0)
                  printf("\t PID: %d\n", blk->pid);
            else
                  printf("\n");

            current = current->next;
            i++;
      }
}

// Main function (Do Not Modify)
int main(int argc, char *argv[]) 
{
      int PARTITION_SIZE, inputdata[200][2], N = 0, Memory_Mgt_Policy;

      list_t *FREE_LIST = list_alloc();  
      list_t *ALLOC_LIST = list_alloc();  

      if (argc != 3) 
      {
            printf("usage: ./mmu <input file> -{F | B | W }\n(F=FIFO | B=BESTFIT | W=WORSTFIT)\n");
            exit(1);
      }

      get_input(argv, inputdata, &N, &PARTITION_SIZE, &Memory_Mgt_Policy);

      block_t *partition = malloc(sizeof(block_t));
      partition->start = 0;
      partition->end = PARTITION_SIZE - 1;
      list_add_to_front(FREE_LIST, partition);

      for (int i = 0; i < N; i++) 
      {
            printf("************************\n");

            if (inputdata[i][0] > 0) 
            {
                  printf("ALLOCATE: %d FROM PID: %d\n", inputdata[i][1], inputdata[i][0]);
                  allocate_memory(FREE_LIST, ALLOC_LIST, inputdata[i][0], inputdata[i][1], Memory_Mgt_Policy);
            } 
            else if (inputdata[i][0] < 0) 
            {
                  printf("DEALLOCATE MEM: PID %d\n", abs(inputdata[i][0]));
                  deallocate_memory(ALLOC_LIST, FREE_LIST, abs(inputdata[i][0]), Memory_Mgt_Policy);
            } 
            else 
            {
                  printf("COALESCE/COMPACT\n");
                  FREE_LIST = coalese_memory(FREE_LIST);
            }

            printf("************************\n");
            print_list(FREE_LIST, "Free Memory");
            print_list(ALLOC_LIST, "\nAllocated Memory\n");
      }

      list_free(FREE_LIST);
      list_free(ALLOC_LIST);

      return 0;
}
