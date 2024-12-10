// Header file for Linked List
// File: list.h

#ifndef LIST_H
#define LIST_H

// Define the structure for a node in the linked list
typedef struct node {
    int data;              // Data stored in the node
    struct node* next;     // Pointer to the next node
} node_t;

// Define the structure for the linked list
typedef struct {
    node_t* head;          // Pointer to the head of the list
} list_t;

// Function declarations for list operations

// Allocate memory for a new list
list_t* list_alloc();

// Free the entire list and release memory
void list_free(list_t* list);

// Add a new element to the front of the list
void list_add_to_front(list_t* list, int value);

// Add a new element to the back of the list
void list_add_to_back(list_t* list, int value);

// Insert an element at a specific index in the list
void list_add_at_index(list_t* list, int index, int value);

// Remove and return the first element from the list
int list_remove_from_front(list_t* list);

// Remove and return the last element from the list
int list_remove_from_back(list_t* list);

// Remove and return the element at a specified index
int list_remove_at_index(list_t* list, int index);

// Get the element at a specified index in the list
int list_get_elem_at(list_t* list, int index);

// Count the number of elements in the list
int list_length(list_t* list);

// Convert the list to a string representation
char* listToString(list_t* list);

#endif // LIST_H
