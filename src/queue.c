#include "queue.h"
#include <stdlib.h>
#include <stdio.h>

Queue* createQueue() {
    Queue *q = (Queue*)malloc(sizeof(Queue));
    if (!q) {
        perror("Failed to allocate queue");
        exit(EXIT_FAILURE);
    }
    q->head = NULL;
    q->tail = NULL;
    q->size = 0;
    return q;
}

void enqueue(Queue *q, void *data) {
    Node *newNode = (Node*)malloc(sizeof(Node));
    if (!newNode) {
        perror("Failed to allocate queue node");
        exit(EXIT_FAILURE);
    }
    newNode->data = data;
    newNode->next = NULL;
    if (q->tail) {
        q->tail->next = newNode;
    }
    q->tail = newNode;
    if (!q->head) {
        q->head = newNode;
    }
    q->size++;
}

void* dequeue(Queue *q) {
    if (is_empty(q)) {
        return NULL;
    }
    Node *temp = q->head;
    void *data = temp->data;
    q->head = q->head->next;
    if (!q->head) {
        q->tail = NULL;
    }
    free(temp);
    q->size--;
    return data;
}

int is_empty(Queue *q) {
    return q->head == NULL;
}

void free_queue(Queue *q, void (*free_data)(void*)) {
    while (!is_empty(q)) {
        void* data = dequeue(q);
        if (free_data) {
            free_data(data);
        }
    }
    free(q);
}
