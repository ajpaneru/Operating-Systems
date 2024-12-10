#include <stdio.h>
#include <stdlib.h>

// Function prototypes for arithmetic operations
int add(int a, int b);
int subtract(int a, int b);
int multiply(int a, int b);
int divide(int a, int b);
void exit_program();

int main(void)
{
      int a = 6, b = 3;   // Initialize variables for calculations
      char choice;

      // Define an array of function pointers
      int (*operations[4])(int, int) = { add, subtract, multiply, divide };
      void (*exit_op)() = exit_program;

      printf("Values: a = %d, b = %d\n", a, b);
      printf("Choose an operation (0 = Add, 1 = Subtract, 2 = Multiply, 3 = Divide, 4 = Exit): ");
      
      scanf(" %c", &choice);

      // Execute the appropriate function based on user input
      if (choice >= '0' && choice <= '3') 
      {
            int result = operations[choice - '0'](a, b);    // Call the selected function
            printf("Result: %d\n", result);
      } 
      else if (choice == '4') 
      {
            exit_op();    // Terminate the program
      }

      return 0;
}

// Function to add two integers
int add(int a, int b) 
{ 
      printf("Performing addition...\n"); 
      return a + b; 
}

// Function to subtract two integers
int subtract(int a, int b) 
{ 
      printf("Performing subtraction...\n"); 
      return a - b; 
}

// Function to multiply two integers
int multiply(int a, int b) 
{ 
      printf("Performing multiplication...\n"); 
      return a * b; 
}

// Function to divide two integers
int divide(int a, int b) 
{ 
      if (b == 0) 
      {
            printf("Error: Division by zero is not allowed.\n");
            return 0;
      }
      printf("Performing division...\n"); 
      return a / b; 
}

// Function to terminate the program
void exit_program() 
{
      printf("Program terminated.\n");
      exit(0);
}
