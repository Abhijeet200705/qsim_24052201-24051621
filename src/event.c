#include "event.h"
#include "simulation.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void log_function_pointer_call(const char* function_name, double clock_time) {
    printf("[LOG] Function Pointer Invoked: %s at simulation time %.2f\n", function_name, clock_time);
}

Event* create_event(double time, ActorType actorType, void *actor, void (*action)(struct Event*, struct Simulation*)) {
    Event *e = (Event*)malloc(sizeof(Event));
    if (!e) {
        perror("Failed to allocate event");
        exit(EXIT_FAILURE);
    }
    e->time = time;
    e->actorType = actorType;
    e->actor = actor;
    e->action = action;
    return e;
}

void add_event_sorted(Queue *event_queue, Event *event) {
    Node *newNode = (Node*)malloc(sizeof(Node));
    if (!newNode) {
        perror("Failed to allocate event queue node");
        exit(EXIT_FAILURE);
    }
    newNode->data = event;
    newNode->next = NULL;

    if (is_empty(event_queue) || ((Event*)event_queue->head->data)->time >= event->time) {
        // Case 1: The queue is empty or the new event is the earliest.
        newNode->next = event_queue->head;
        event_queue->head = newNode;
        if (event_queue->tail == NULL) { // If queue was empty, new node is also the tail
            event_queue->tail = newNode;
        }
    } else {
        // Case 2: Find the correct position to insert the new event.
        Node *current = event_queue->head;
        while (current->next != NULL && ((Event*)current->next->data)->time < event->time) {
            current = current->next;
        }
        newNode->next = current->next;
        current->next = newNode;
        if (newNode->next == NULL) { // If inserted at the end, update the tail
            event_queue->tail = newNode;
        }
    }

    event_queue->size++;
}

void handle_customer_arrival(Event *event, Simulation *sim) {
    log_function_pointer_call("handle_customer_arrival", sim->clock);
    Customer *customer = (Customer*)event->actor;

    Queue* target_queue;

    if (sim->config.queueType == SINGLE_QUEUE) {
        target_queue = sim->tellerQueues[0];
    } else { // MULTIPLE_QUEUES
        // Find all shortest queues and pick one at random
        int shortest_indices[sim->config.numTellers]; // Array to hold indices of shortest queues
        shortest_indices[0] = 0;
        int shortest_count = 1;
        int min_len = sim->tellerQueues[0]->size;

        for (int i = 1; i < sim->config.numTellers; i++) {
            if (sim->tellerQueues[i]->size < min_len) {
                min_len = sim->tellerQueues[i]->size;
                shortest_count = 1; // New shortest found, reset candidates
                shortest_indices[0] = i;
            } else if (sim->tellerQueues[i]->size == min_len) {
                shortest_indices[shortest_count++] = i; // Add to list of tied shortest queues
            }
        }
        // Select a random queue from the list of shortest ones
        int choice = rand() % shortest_count;
        int shortest_queue_index = shortest_indices[choice];
        target_queue = sim->tellerQueues[shortest_queue_index];
    }

    // Check if the chosen queue is too long
    if (target_queue->size >= MAX_QUEUE_LENGTH) {
        // Customer balks
        sim->customersBalked++;
        // This customer leaves immediately and is not served.
    } else {
        // Customer joins the queue
        enqueue(target_queue, customer);
    }
}

void handle_service_completion(Event *event, Simulation *sim) {
    log_function_pointer_call("handle_service_completion", sim->clock);
    Customer *customer = (Customer*)event->actor;
    customer->departureTime = event->time;

    double timeInBank = customer->departureTime - customer->arrivalTime;
    double waitTime = customer->serviceStartTime - customer->arrivalTime;

    sim->customersServed++;
    sim->totalCustomerTimeInBank += timeInBank;
    sim->totalCustomerWaitTime += waitTime;
    if (waitTime > sim->maxWaitTime) {
        sim->maxWaitTime = waitTime;
    }
    sim->customerTimesInBank[customer->id] = timeInBank;

    // Customer leaves, no need to free actor here as it's part of a larger array
}

/**
 * @brief Finds the next available customer for a teller.
 * For a single queue, it takes from the common line.
 * For multiple queues, it checks its own line first, then steals from another.
 * @return A pointer to a Customer, or NULL if no customers are waiting.
 */
static Customer* find_customer_for_teller(Simulation *sim, Teller *teller) {
    if (sim->config.queueType == SINGLE_QUEUE) {
        if (!is_empty(sim->tellerQueues[0])) {
            return (Customer*)dequeue(sim->tellerQueues[0]);
        }
    } else { // MULTIPLE_QUEUES
        // First check own queue
        if (teller->id < sim->config.numTellers && !is_empty(sim->tellerQueues[teller->id])) {
            return (Customer*)dequeue(sim->tellerQueues[teller->id]);
        }
        // If own queue is empty, steal from another random queue
        int random_start = rand() % sim->config.numTellers;
        for (int i = 0; i < sim->config.numTellers; i++) {
            int idx = (random_start + i) % sim->config.numTellers;
            if (!is_empty(sim->tellerQueues[idx])) {
                return (Customer*)dequeue(sim->tellerQueues[idx]);
            }
        }
    }
    return NULL; // No customer found
}

void handle_teller_becomes_free(Event *event, Simulation *sim) {
    log_function_pointer_call("handle_teller_becomes_free", sim->clock);
    Teller *teller = (Teller*)event->actor;
    teller->isIdle = 1;

    Customer *customer_to_serve = find_customer_for_teller(sim, teller);

    if (customer_to_serve) {
        teller->isIdle = 0;
        double serviceTime = SERVICE_TIME_DISTRIBUTION_FACTOR * sim->config.averageServiceTime * rand() / (double)RAND_MAX;
        double completionTime = sim->clock + serviceTime;

        customer_to_serve->serviceStartTime = sim->clock;
        teller->totalServiceTime += serviceTime;

        // Event for when customer is done
        add_event_sorted(sim->eventQueue, create_event(completionTime, ACTOR_CUSTOMER, customer_to_serve, handle_service_completion));
        // Event for when teller is free again
        add_event_sorted(sim->eventQueue, create_event(completionTime, ACTOR_TELLER, teller, handle_teller_becomes_free));

    } else { // No customers anywhere, teller becomes idle
        // Only schedule a new idle event if there are still customers to be served.
        // Otherwise, the teller becomes permanently idle, allowing the simulation to end.
        if ((sim->customersServed + sim->customersBalked) < sim->config.numCustomers) {
            double idleTime = (MIN_TELLER_IDLE_SECONDS + rand() % (MAX_TELLER_IDLE_SECONDS - MIN_TELLER_IDLE_SECONDS + 1)) / 60.0;
            double nextCheckTime = sim->clock + idleTime;
            teller->totalIdleTime += idleTime;

            // Event for teller to check again after idling
            add_event_sorted(sim->eventQueue, create_event(nextCheckTime, ACTOR_TELLER, teller, handle_teller_becomes_free));
        }
    }
}
