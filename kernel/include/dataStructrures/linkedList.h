#include <memory.h>

// Define a generic linked list node
typedef struct linkedListNode {
    void *data;              // Pointer to hold data of any type
    struct Node *next;       // Pointer to the next node
} linkedListNode;

// Create a new node
linkedListNode* create_node(void *data) {
    linkedListNode *new_node = (linkedListNode *)malloc(sizeof(linkedListNode));
    if (new_node) {
        new_node->data = data;
        new_node->next = NULL;
    }
    return new_node;
}

// Add a node to the end of the list
void append_node(linkedListNode **head, void *data) {
    linkedListNode *new_node = create_node(data);
    if (*head == NULL) {
        *head = new_node;
    } else {
        linkedListNode *current = *head;
        while (current->next != NULL) {
            current = current->next;
        }
        current->next = new_node;
    }
}

// Remove a node from the list
void remove_node(linkedListNode **head, void *data, int (*cmp)(void *, void *)) {
    linkedListNode *current = *head;
    linkedListNode *previous = NULL;
    while (current != NULL) {
        if (cmp(current->data, data) == 0) {
            if (previous == NULL) {
                *head = current->next;
            } else {
                previous->next = current->next;
            }
            free(current);
            return;
        }
        previous = current;
        current = current->next;
    }
}

// Free the entire list
void free_list(linkedListNode *head) {
    linkedListNode *current = head;
    linkedListNode *next_node;
    while (current != NULL) {
        next_node = current->next;
        free(current);
        current = next_node;
    }
}