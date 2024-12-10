// Demonstration of Two-Way Communication Between Processes Using Pipes
#include <stdio.h>    // For standard input/output functions
#include <stdlib.h>   // For exit(), memory management functions
#include <unistd.h>   // For pipe(), fork(), and process control functions
#include <sys/types.h> 
#include <string.h>   // For string handling functions
#include <sys/wait.h> // For wait() function

int main() 
{ 
    int fd1[2];  // Pipe 1: Parent writes, Child reads
    int fd2[2];  // Pipe 2: Child writes, Parent reads
  
    char fixed_str_child[] = "howard.edu"; 
    char fixed_str_parent[] = "gobison.org"; 
    char input_str[100]; 
    char input_str_child[100]; 
    pid_t p; 

    // Create pipes and handle failures
    if (pipe(fd1) == -1) 
    { 
        fprintf(stderr, "Failed to create Pipe 1\n"); 
        return 1; 
    } 
    if (pipe(fd2) == -1) 
    { 
        fprintf(stderr, "Failed to create Pipe 2\n"); 
        return 1; 
    } 

    // Get input from the user in the parent process
    printf("Enter a string to concatenate: ");
    scanf("%s", input_str); 
  
    p = fork(); // Create a child process
  
    if (p < 0) 
    { 
        fprintf(stderr, "Fork failed\n"); 
        return 1; 
    } 
  
    // Parent process (P1)
    else if (p > 0) 
    { 
        close(fd1[0]);  // Close read-end of Pipe 1 (Parent writes)
        close(fd2[1]);  // Close write-end of Pipe 2 (Parent reads)
  
        // Send input string to child through Pipe 1
        write(fd1[1], input_str, strlen(input_str) + 1); 
        close(fd1[1]); // Close write-end after writing
  
        wait(NULL); // Wait for child to finish execution
  
        // Read concatenated string from child through Pipe 2
        char concat_str_child[200];
        read(fd2[0], concat_str_child, 200);
        printf("Concatenated string from child: %s\n", concat_str_child);

        // Append parent's fixed string and print final result
        strcat(concat_str_child, fixed_str_parent);
        printf("Final concatenated string with parent suffix: %s\n", concat_str_child);
  
        close(fd2[0]); // Close read-end of Pipe 2
    } 
  
    // Child process (P2)
    else
    { 
        close(fd1[1]);  // Close write-end of Pipe 1 (Child reads)
        close(fd2[0]);  // Close read-end of Pipe 2 (Child writes)
  
        // Read string sent by parent through Pipe 1
        char concat_str[100]; 
        read(fd1[0], concat_str, 100); 
        close(fd1[0]); // Close read-end after reading
  
        // Append child's fixed string and print result
        strcat(concat_str, fixed_str_child);
        printf("Concatenated string in child: %s\n", concat_str);
  
        // Send concatenated string back to parent through Pipe 2
        write(fd2[1], concat_str, strlen(concat_str) + 1);
  
        // Get another input from the user in the child process
        printf("Enter another string to send to parent: ");
        scanf("%s", input_str_child);

        // Send additional input string to parent
        write(fd2[1], input_str_child, strlen(input_str_child) + 1);
        close(fd2[1]); // Close write-end of Pipe 2
  
        exit(0); // Terminate child process
    } 
  
    return 0; 
}
