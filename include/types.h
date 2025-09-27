#ifndef TYPES_H
#define TYPES_H

#include <stdio.h>

// --- Constants ---
#define MIN_TELLER_IDLE_SECONDS 1
#define MAX_TELLER_IDLE_SECONDS 150
#define MAX_QUEUE_LENGTH 8
#define SERVICE_TIME_DISTRIBUTION_FACTOR 2.0

// --- Enums ---

// Type of actor associated with an event
typedef enum {
    ACTOR_CUSTOMER,
    ACTOR_TELLER
} ActorType;

// Type of queuing system being simulated
typedef enum {
    SINGLE_QUEUE,
    MULTIPLE_QUEUES
} QueueType;

// --- Structures ---

typedef struct Customer {
    int id;
    double arrivalTime;
    double serviceStartTime;
    double departureTime;
} Customer;

typedef struct Teller {
    int id;
    int isIdle;
    double totalServiceTime;
    double totalIdleTime;
} Teller;


#endif // TYPES_H
