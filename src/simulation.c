#include "simulation.h"
#include "event.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>

Simulation* create_simulation(const SimConfig *config) {
    Simulation *sim = (Simulation*)malloc(sizeof(Simulation));
    if (!sim) {
        perror("Failed to allocate simulation");
        exit(EXIT_FAILURE);
    }

    sim->config = *config; // Copy the configuration

    sim->clock = 0.0;
    sim->eventQueue = createQueue();

    int num_queues = (config->queueType == SINGLE_QUEUE) ? 1 : config->numTellers;
    sim->tellerQueues = (Queue**)malloc(num_queues * sizeof(Queue*));
    for (int i = 0; i < num_queues; i++) {
        sim->tellerQueues[i] = createQueue();
    }
    sim->customers = (Customer*)malloc(config->numCustomers * sizeof(Customer));
    sim->tellers = (Teller*)malloc(config->numTellers * sizeof(Teller));
    sim->customerTimesInBank = (double*)calloc(config->numCustomers, sizeof(double));

    sim->customersServed = 0;
    sim->customersBalked = 0;
    sim->totalCustomerTimeInBank = 0.0;
    sim->totalCustomerWaitTime = 0.0;
    sim->maxWaitTime = 0.0;

    return sim;
}

void initialize_simulation(Simulation *sim) {
    // Create customer arrival events
    for (int i = 0; i < sim->config.numCustomers; i++) {
        sim->customers[i].id = i;
        sim->customers[i].arrivalTime = sim->config.simulationTime * rand() / (double)RAND_MAX;
        sim->customers[i].serviceStartTime = -1;
        sim->customers[i].departureTime = -1;

        Event *arrival_event = create_event(sim->customers[i].arrivalTime, ACTOR_CUSTOMER, &sim->customers[i], handle_customer_arrival);
        add_event_sorted(sim->eventQueue, arrival_event);
    }

    // Create initial teller events
    for (int i = 0; i < sim->config.numTellers; i++) {
        sim->tellers[i].id = i;
        sim->tellers[i].isIdle = 1;
        sim->tellers[i].totalServiceTime = 0.0;
        sim->tellers[i].totalIdleTime = 0.0;

        // Tellers are available at time 0 to start looking for customers
        Event *teller_event = create_event(0.0, ACTOR_TELLER, &sim->tellers[i], handle_teller_becomes_free);
        add_event_sorted(sim->eventQueue, teller_event);
    }
}

void run_simulation(Simulation *sim) {
    while (!is_empty(sim->eventQueue)) {
        Event *current_event = (Event*)dequeue(sim->eventQueue);

        // Advance clock
        if (current_event->time > sim->clock) {
            sim->clock = current_event->time;
        }

        // If simulation time is exceeded, we might stop processing new arrivals,
        // but we must continue until all existing customers are served.
        // This implementation processes all events.

        // Invoke event action via function pointer
        current_event->action(current_event, sim);

        free(current_event);
    }
}

void print_statistics(Simulation *sim) {
    printf("\n--- Simulation Statistics ---\n");
    printf("Queueing Type: %s\n", sim->config.queueType == SINGLE_QUEUE ? "Single Common Queue" : "One Queue Per Teller");
    printf("Number of Tellers: %d\n", sim->config.numTellers);
    printf("--------------------------------\n");

    if (sim->customersServed == 0) {
        printf("No customers were served.\n");
        return;
    }

    double avgTimeInBank = sim->totalCustomerTimeInBank / sim->customersServed;

    // Calculate standard deviation
    double sum_sq_diff = 0.0;
    for (int i = 0; i < sim->config.numCustomers; i++) {
        if (sim->customerTimesInBank[i] > 0) { // Only for served customers
            sum_sq_diff += pow(sim->customerTimesInBank[i] - avgTimeInBank, 2);
        }
    }
    double std_dev = sqrt(sum_sq_diff / sim->customersServed);

    double total_teller_service_time = 0;
    double total_teller_idle_time = 0;
    for(int i = 0; i < sim->config.numTellers; i++) {
        total_teller_service_time += sim->tellers[i].totalServiceTime;
        total_teller_idle_time += sim->tellers[i].totalIdleTime;
    }

    printf("Total Customers Served: %d\n", sim->customersServed);
    printf("Total Customers Balked (left without joining queue): %d\n", sim->customersBalked);
    printf("Total Simulation Time (to serve all): %.2f minutes\n", sim->clock);
    printf("Average Time in Bank: %.2f minutes\n", avgTimeInBank);
    printf("Standard Deviation of Time in Bank: %.2f minutes\n", std_dev);
    printf("Maximum Customer Wait Time: %.2f minutes\n", sim->maxWaitTime);
    printf("Total Teller Service Time: %.2f minutes\n", total_teller_service_time);
    printf("Total Teller Idle Time: %.2f minutes\n", total_teller_idle_time);
    printf("--------------------------------\n");
}

void cleanup_simulation(Simulation *sim) {
    if (!sim) return;

    // Free event queue. Pass 'free' as the function to clean up the Event* data.
    free_queue(sim->eventQueue, free);

    // Free teller queues
    // Pass NULL because the Customer* data is part of a larger array and should not be freed here.
    int num_queues = (sim->config.queueType == SINGLE_QUEUE) ? 1 : sim->config.numTellers;
    for (int i = 0; i < num_queues; i++) {
        free_queue(sim->tellerQueues[i], NULL);
    }
    free(sim->tellerQueues);

    // Free main data arrays
    free(sim->customers);
    free(sim->tellers);
    free(sim->customerTimesInBank);

    // Free the simulation struct itself
    free(sim);
}
