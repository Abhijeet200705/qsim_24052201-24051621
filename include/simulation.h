#ifndef SIMULATION_H
#define SIMULATION_H

#include "types.h"
#include "queue.h"

// Structure to hold simulation configuration parameters
typedef struct SimConfig {
    int numCustomers;
    int numTellers;
    double simulationTime;
    double averageServiceTime;
    QueueType queueType;
} SimConfig;

// Structure to hold all simulation parameters and state
typedef struct Simulation {
    SimConfig config;

    // State
    double clock;
    Queue *eventQueue;
    Queue **tellerQueues; // Array of queues for tellers
    Customer *customers;
    Teller *tellers;

    // Statistics
    int customersServed;
    int customersBalked;
    double totalCustomerTimeInBank;
    double totalCustomerWaitTime;
    double maxWaitTime;
    double *customerTimesInBank; // For standard deviation

} Simulation;

Simulation* create_simulation(const SimConfig *config);
void initialize_simulation(Simulation *sim);
void run_simulation(Simulation *sim);
void print_statistics(Simulation *sim);
void cleanup_simulation(Simulation *sim);


#endif // SIMULATION_H
