// Linked List Module Implementation
// File: list/list.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "list.h"

// Function to allocate memory for a new list
list_t *list_alloc() 
{ 
    list_t* list = (list_t*)malloc(sizeof(list_t));
    list->head = NULL;
    return list; 
}

// Function to create a new node with a given block
node_t *node_alloc(block_t *blk) 
{   
    node_t* node = (node_t*)malloc(sizeof(node_t));
    node->next = NULL;
    node->blk = blk;
    return node; 
}

// Free memory associated with the entire list
void list_free(list_t *l)
{
    free(l);
}

// Free memory associated with a specific node
void node_free(node_t *node)
{
    free(node);
}

// Print all elements currently in the list
void list_print(list_t *l) 
{
    node_t *current = l->head;

    if (current == NULL)
    {
        printf("List is empty\n");
        return;
    }

    while (current != NULL)
    {
        block_t *b = current->blk;
        printf("PID=%d START:%d END:%d\n", b->pid, b->start, b->end);
        current = current->next;
    }
}

// Calculate the length of the list
int list_length(list_t *l) 
{ 
    node_t *current = l->head;
    int count = 0;

    while (current != NULL)
    {
        count++;
        current = current->next;
    }
    return count; 
}

// Add a block to the end of the list
void list_add_to_back(list_t *l, block_t *blk)
{  
    node_t* newNode = node_alloc(blk);

    if (l->head == NULL)
    {
        l->head = newNode;
    }
    else
    {
        node_t *current = l->head;

        while (current->next != NULL)
        {
            current = current->next;
        }
        current->next = newNode;
    }
}

// Insert a block at the beginning of the list
void list_add_to_front(list_t *l, block_t *blk)
{  
    node_t* newNode = node_alloc(blk);
    newNode->next = l->head;
    l->head = newNode;
}

// Insert a block at a specified index
void list_add_at_index(list_t *l, block_t *blk, int index)
{
    int i = 0;
    node_t *newNode = node_alloc(blk);
    node_t *current = l->head;

    if (index == 0)
    {
        newNode->next = l->head;
        l->head = newNode;
    }
    else
    {
        while (i < index - 1 && current != NULL)
        {
            current = current->next;
            i++;
        }
        newNode->next = current->next;
        current->next = newNode;
    }
}

// Add a block in ascending order by address
void list_add_ascending_by_address(list_t *l, block_t *newblk)
{
    node_t *current = l->head;
    node_t *prev = NULL;
    node_t *newNode = node_alloc(newblk);

    while (current != NULL && current->blk->start < newblk->start)
    {
        prev = current;
        current = current->next;
    }

    if (prev == NULL)
    {
        newNode->next = l->head;
        l->head = newNode;
    }
    else
    {
        prev->next = newNode;
        newNode->next = current;
    }
}

// Add a block in ascending order by block size
void list_add_ascending_by_blocksize(list_t *l, block_t *newblk)
{
    int newblk_size = newblk->end - newblk->start + 1;
    node_t *current = l->head;
    node_t *prev = NULL;
    node_t *newNode = node_alloc(newblk);

    while (current != NULL)
    {
        int currblk_size = current->blk->end - current->blk->start + 1;

        if (newblk_size < currblk_size)
        {
            break;
        }

        prev = current;
        current = current->next;
    }

    if (prev == NULL)
    {
        newNode->next = l->head;
        l->head = newNode;
    }
    else
    {
        prev->next = newNode;
        newNode->next = current;
    }
}

// Add a block in descending order by block size
void list_add_descending_by_blocksize(list_t *l, block_t *blk)
{
    node_t *current = l->head;
    node_t *prev = NULL;
    node_t *newNode = node_alloc(blk);
    int newblk_size = blk->end - blk->start + 1;

    if (l->head == NULL)
    {
        l->head = newNode;
        return;
    }

    while (current != NULL && (current->blk->end - current->blk->start + 1) >= newblk_size)
    {
        prev = current;
        current = current->next;
    }

    if (prev == NULL)
    {
        newNode->next = l->head;
        l->head = newNode;
    }
    else
    {
        prev->next = newNode;
        newNode->next = current;
    }
}

// Merge adjacent free blocks in the list
void list_coalese_nodes(list_t *l)
{
    node_t *current = l->head;

    while (current != NULL && current->next != NULL)
    {
        if (current->blk->end + 1 == current->next->blk->start)
        {
            current->blk->end = current->next->blk->end;
            node_t *temp = current->next;
            current->next = current->next->next;
            node_free(temp);
        }
        else
        {
            current = current->next;
        }
    }
}

// Remove and return the last block from the list
block_t* list_remove_from_back(list_t *l)
{
    if (l->head == NULL)
    {
        return NULL;
    }

    node_t *current = l->head;
    node_t *prev = NULL;

    while (current->next != NULL)
    {
        prev = current;
        current = current->next;
    }

    block_t *value = current->blk;

    if (prev != NULL)
    {
        prev->next = NULL;
    }
    else
    {
        l->head = NULL;
    }

    node_free(current);
    return value;
}

// Retrieve the first block from the list without removing it
block_t* list_get_from_front(list_t *l)
{
    return (l->head == NULL) ? NULL : l->head->blk;
}

// Remove and return the first block from the list
block_t* list_remove_from_front(list_t *l)
{
    if (l->head == NULL)
    {
        return NULL;
    }

    node_t *temp = l->head;
    block_t *value = temp->blk;
    l->head = l->head->next;
    node_free(temp);
    return value;
}
// Remove a block from a specific index in the list
block_t* list_remove_at_index(list_t *l, int index) 
{ 
    int i = 0;
    block_t* value = NULL;
    node_t *current = l->head;
    node_t *prev = NULL;

    if (l->head == NULL) 
    {
        return NULL;
    }
    else if (index == 0) 
    {
        return list_remove_from_front(l);
    }
    else 
    {
        while (current != NULL && i < index) 
        {
            prev = current;
            current = current->next;
            i++;
        }
        if (current != NULL) 
        {
            value = current->blk;
            prev->next = current->next;
            node_free(current);
        }
    }
    return value; 
}

// Compare two blocks for equality based on PID, start, and end
bool compareBlks(block_t* a, block_t *b) 
{
    return (a->pid == b->pid && a->start == b->start && a->end == b->end);
}

// Check if a block has a size equal to or greater than the specified size
bool compareSize(int size, block_t *blk) 
{  
    return (size <= (blk->end - blk->start + 1));
}

// Check if a block has the specified PID
bool comparePid(int pid, block_t *blk) 
{
    return (pid == blk->pid);
}

// Check if a block exists in the list
bool list_is_in(list_t *l, block_t* value) 
{ 
    node_t *current = l->head;

    while (current != NULL) 
    {
        if (compareBlks(value, current->blk)) 
        {
            return true;
        }
        current = current->next;
    }
    return false; 
}

// Retrieve a block from a specific index
block_t* list_get_elem_at(list_t *l, int index) 
{ 
    int i = 0;
    block_t* value = NULL;
    node_t *current = l->head;

    if (l->head == NULL) 
    {
        return NULL;
    }
    else 
    {
        while (current != NULL) 
        {
            if (i == index) 
            {
                return current->blk;
            }
            current = current->next;
            i++;
        }
    }
    return value; 
}

// Get the index of a specific block
int list_get_index_of(list_t *l, block_t* value) 
{
    int i = 0;
    node_t *current = l->head;

    while (current != NULL) 
    {
        if (compareBlks(value, current->blk)) 
        {
            return i;
        }
        current = current->next;
        i++;
    }
    return -1; 
}

// Check if a block of a specific size exists in the list
bool list_is_in_by_size(list_t *l, int size) 
{ 
    node_t *current = l->head;

    while (current != NULL) 
    {
        if (compareSize(size, current->blk)) 
        {
            return true;
        }
        current = current->next;
    }
    return false; 
}

// Check if a block with a specific PID exists in the list
bool list_is_in_by_pid(list_t *l, int pid) 
{
    node_t *current = l->head;

    while (current != NULL) 
    {
        if (comparePid(pid, current->blk)) 
        {
            return true;
        }
        current = current->next;
    }
    return false;
}

// Get the index of the first block with the specified size or greater
int list_get_index_of_by_Size(list_t *l, int size) 
{
    int i = 0;
    node_t *current = l->head;

    while (current != NULL) 
    {
        if (compareSize(size, current->blk)) 
        {
            return i;
        }
        current = current->next;
        i++;
    }
    return -1; 
}

// Get the index of the first block with the specified PID
int list_get_index_of_by_Pid(list_t *l, int pid) 
{
    int i = 0;
    node_t *current = l->head;

    while (current != NULL) 
    {
        if (comparePid(pid, current->blk)) 
        {
            return i;
        }
        current = current->next;
        i++;
    }
    return -1; 
}
