// C program to demonstrate use of fork() and pipe() 
#include<stdio.h> 
#include<stdlib.h> 
#include<unistd.h> 
#include<sys/types.h> 
#include<string.h> 
#include<sys/wait.h> 
  
int main() 
{ 
    // We use two pipes 
    // First pipe to send input string from parent to child 
    // Second pipe to send concatenated string from child to parent
  
    int fd1[2];  // Used to store two ends of first pipe 
    int fd2[2];  // Used to store two ends of second pipe 
  
    char fixed_str1[] = "howard.edu"; 
    char fixed_str2[] = "gobison.org"; 
    char input_str[100]; 
    char second_input_str[100]; 
    pid_t p; 
  
    if (pipe(fd1) == -1) 
    { 
        fprintf(stderr, "Pipe Failed" ); 
        return 1; 
    } 
    if (pipe(fd2) == -1) 
    { 
        fprintf(stderr, "Pipe Failed" ); 
        return 1; 
    } 
  
    printf("Enter a string to concatenate: ");
    scanf("%s", input_str); 
    p = fork(); 
  
    if (p < 0) 
    { 
        fprintf(stderr, "Fork Failed" ); 
        return 1; 
    } 
  
    // Parent process 
    else if (p > 0) 
    { 
        close(fd1[0]);  // Close reading end of first pipe 
        close(fd2[1]);  // Close writing end of second pipe
  
        // Write input string and close writing end of first pipe
        write(fd1[1], input_str, strlen(input_str) + 1); 
        close(fd1[1]); // Close writing end after sending
  
        // Wait for child to finish
        wait(NULL); 
  
        // Read the concatenated string from child (P2)
        read(fd2[0], input_str, 100);
        close(fd2[0]); // Close reading end
  
        // Concatenate "gobison.org" and print final output
        strcat(input_str, fixed_str2); 
        printf("Final output: %s\n", input_str);
    } 
  
    // Child process 
    else
    { 
        close(fd1[1]);  // Close writing end of first pipe 
        close(fd2[0]);  // Close reading end of second pipe
  
        // Read the string from parent (P1)
        read(fd1[0], second_input_str, 100);
        close(fd1[0]); // Close reading end after reading
  
        // Concatenate "howard.edu" to the string received from P1
        strcat(second_input_str, fixed_str1); 
        printf("Concatenated string: %s\n", second_input_str);
  
        // Ask the user for another string
        printf("Enter another string: ");
        scanf("%s", input_str);
  
        // Write the concatenated string to parent (P1)
        write(fd2[1], second_input_str, strlen(second_input_str) + 1);
        close(fd2[1]); // Close writing end after sending
  
        exit(0); 
    } 
    return 0;
}
