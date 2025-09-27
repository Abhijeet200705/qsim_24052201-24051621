#ifndef QUEUE_H
#define QUEUE_H

#include "types.h"

// A generic node for a linked list
typedef struct Node {
    void *data;
    struct Node *next;
} Node;

// A generic queue structure
typedef struct Queue {
    Node *head;
    Node *tail;
    int size;
} Queue;

Queue* createQueue();
void enqueue(Queue *q, void *data); // Add to tail
void* dequeue(Queue *q); // Remove from head
int is_empty(Queue *q);
void free_queue(Queue *q, void (*free_data)(void*));

#endif // QUEUE_H
