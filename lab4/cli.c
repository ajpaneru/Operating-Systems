// Client Implementation for Socket Communication
// File: client.c

#include <netinet/in.h>   // For sockaddr_in structure
#include <stdio.h>        // For standard I/O functions
#include <stdlib.h>       // For exit(), atoi()
#include <string.h>       // For string manipulation functions
#include <sys/socket.h>   // For socket-related system calls
#include <sys/types.h>    // For socket types
#include <unistd.h>       // For close() function

#define PORT 9001                  // Server port number
#define MAX_COMMAND_LINE_LEN 1024 // Maximum length for command line input

// Function to read a command line from standard input
char* getCommandLine(char *command_line) 
{
    do 
    {
        // Read input from the user and check for errors
        if ((fgets(command_line, MAX_COMMAND_LINE_LEN, stdin) == NULL) && ferror(stdin)) 
        {
            fprintf(stderr, "Error reading input\n");
            exit(1);
        }
    } while (command_line[0] == '\n');  // Ignore empty input lines

    // Remove the trailing newline character
    command_line[strlen(command_line) - 1] = '\0';  
    return command_line;
}

int main(int argc, char const* argv[]) 
{
    // Create a socket
    int sockID = socket(AF_INET, SOCK_STREAM, 0);
    if (sockID < 0) 
    {
        perror("Failed to create socket");
        exit(1);
    }

    char buf[MAX_COMMAND_LINE_LEN];      // Buffer for sending commands
    char responeData[MAX_COMMAND_LINE_LEN]; // Buffer for receiving responses
    struct sockaddr_in servAddr;         // Server address structure

    // Initialize the server address structure
    servAddr.sin_family = AF_INET;
    servAddr.sin_port = htons(PORT);
    servAddr.sin_addr.s_addr = INADDR_ANY;

    // Attempt to connect to the server
    if (connect(sockID, (struct sockaddr*)&servAddr, sizeof(servAddr)) < 0) 
    {
        perror("Failed to connect to server");
        exit(1);
    }

    printf("Connected to the server!\n");

    // Main loop to send commands and receive responses
    while (1) 
    {
        printf("Enter Command (or menu): ");
        getCommandLine(buf);  // Get the command from the user

        // Send the command to the server
        send(sockID, buf, strlen(buf), 0);

        // Handle the "exit" command
        if (strcmp(buf, "exit") == 0) 
        {
            printf("Client exiting...\n");
            close(sockID);  // Properly close the socket
            exit(0);
        } 
        // Display the command menu if "menu" is entered
        else if (strcmp(buf, "menu") == 0) 
        {
            printf("AVAILABLE COMMANDS:\n--------------------\n");
            printf("1. print\n");
            printf("2. get_length\n");
            printf("3. add_back <value>\n");
            printf("4. add_front <value>\n");
            printf("5. add_position <index> <value>\n");
            printf("6. remove_back\n");
            printf("7. remove_front\n");
            printf("8. remove_position <index>\n");
            printf("9. get <index>\n");
            printf("10. exit\n");
        }

        // Receive and display the server's response
        recv(sockID, responeData, sizeof(responeData), 0);
        printf("\nSERVER RESPONSE: %s\n", responeData);

        // Clear the buffer for the next command
        memset(buf, 0, MAX_COMMAND_LINE_LEN);
    }

    return 0;
}
