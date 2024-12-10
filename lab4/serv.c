// Server-side Linked List Implementation
// File: server.c

#include <netinet/in.h>   // For sockaddr_in structure
#include <stdio.h>        // For standard I/O functions
#include <string.h>       // For string handling functions
#include <stdlib.h>       // For memory management functions
#include <sys/socket.h>   // For socket functions
#include <sys/types.h>    // For socket types
#include <unistd.h>       // For close() function

#include "list.h"         // Include linked list functions

#define PORT 9001         // Server's listening port
#define ACK "ACK"         // Acknowledgment message

int main(int argc, char const* argv[]) 
{
    int n, val, idx;            // Variables for parsing commands
    int servSockD = socket(AF_INET, SOCK_STREAM, 0);  // Create server socket

    if (servSockD < 0) 
    {
        perror("Socket creation failed");
        exit(1);
    }

    char buf[1024];             // Buffer for client requests
    char sbuf[1024];            // Buffer for server responses
    char* token;                // Token for parsing client commands

    struct sockaddr_in servAddr;
    list_t *mylist = list_alloc();  // Allocate memory for the linked list

    // Configure server address
    servAddr.sin_family = AF_INET;
    servAddr.sin_port = htons(PORT);
    servAddr.sin_addr.s_addr = INADDR_ANY;

    // Bind the server socket to the address and port
    if (bind(servSockD, (struct sockaddr*)&servAddr, sizeof(servAddr)) < 0) 
    {
        perror("Bind failed");
        exit(1);
    }

    // Start listening for incoming connections
    if (listen(servSockD, 1) < 0) 
    {
        perror("Listen failed");
        exit(1);
    }

    printf("Waiting for a client to connect...\n");

    // Accept incoming client connection
    int clientSocket = accept(servSockD, NULL, NULL);
    if (clientSocket < 0) 
    {
        perror("Client connection failed");
        exit(1);
    }
    printf("Client connected!\n");

    // Main server loop for processing client requests
    while (1) 
    {
        memset(buf, 0, sizeof(buf));  // Clear command buffer

        n = recv(clientSocket, buf, sizeof(buf), 0);  // Receive client request
        buf[n] = '\0';  // Null-terminate the received string

        if (n > 0) 
        {
            token = strtok(buf, " ");  // Extract command token
            memset(sbuf, 0, sizeof(sbuf));  // Clear response buffer

            // Handle different client commands
            if (strcmp(token, "exit") == 0) 
            {
                list_free(mylist);  // Free allocated memory
                printf("Server shutting down...\n");
                close(clientSocket);  // Close client socket
                close(servSockD);     // Close server socket
                exit(0);
            } 
            else if (strcmp(token, "print") == 0) 
            {
                snprintf(sbuf, sizeof(sbuf), "%s", listToString(mylist));
            } 
            else if (strcmp(token, "get_length") == 0) 
            {
                val = list_length(mylist);
                snprintf(sbuf, sizeof(sbuf), "Length = %d", val);
            } 
            else if (strcmp(token, "add_back") == 0) 
            {
                token = strtok(NULL, " ");
                val = atoi(token);
                list_add_to_back(mylist, val);
                snprintf(sbuf, sizeof(sbuf), "%s %d", ACK, val);
            } 
            else if (strcmp(token, "add_front") == 0) 
            {
                token = strtok(NULL, " ");
                val = atoi(token);
                list_add_to_front(mylist, val);
                snprintf(sbuf, sizeof(sbuf), "%s %d", ACK, val);
            } 
            else if (strcmp(token, "add_position") == 0) 
            {
                token = strtok(NULL, " ");
                idx = atoi(token);
                token = strtok(NULL, " ");
                val = atoi(token);
                list_add_at_index(mylist, idx, val);
                snprintf(sbuf, sizeof(sbuf), "%s %d at %d", ACK, val, idx);
            } 
            else if (strcmp(token, "remove_back") == 0) 
            {
                val = list_remove_from_back(mylist);
                snprintf(sbuf, sizeof(sbuf), "Removed = %d", val);
            } 
            else if (strcmp(token, "remove_front") == 0) 
            {
                val = list_remove_from_front(mylist);
                snprintf(sbuf, sizeof(sbuf), "Removed = %d", val);
            } 
            else if (strcmp(token, "remove_position") == 0) 
            {
                token = strtok(NULL, " ");
                idx = atoi(token);
                val = list_remove_at_index(mylist, idx);
                snprintf(sbuf, sizeof(sbuf), "Removed = %d from %d", val, idx);
            } 
            else if (strcmp(token, "get") == 0) 
            {
                token = strtok(NULL, " ");
                idx = atoi(token);
                val = list_get_elem_at(mylist, idx);
                snprintf(sbuf, sizeof(sbuf), "Element at %d = %d", idx, val);
            } 
            else 
            {
                snprintf(sbuf, sizeof(sbuf), "Unknown command");
            }

            // Send response back to the client
            send(clientSocket, sbuf, strlen(sbuf) + 1, 0);  
        }
    }

    return 0;
}
