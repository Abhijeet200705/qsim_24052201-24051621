#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "simulation.h"

// Forward declaration for the helper function
static Simulation* run_and_get_stats(const SimConfig* config);

/**
 * @brief Runs a simulation with the given configuration and prints statistics.
 * @param config The simulation configuration.
 * @return A pointer to the completed simulation state for further analysis.
 */
static Simulation* run_and_get_stats(const SimConfig* config) {
    printf("\n--- Starting Simulation: %s ---\n", config->queueType == SINGLE_QUEUE ? "Single Common Queue" : "Multiple Queues");
    Simulation *sim = create_simulation(config);
    initialize_simulation(sim);
    run_simulation(sim);
    print_statistics(sim);
    return sim;
}

int main(int argc, char *argv[]) {
    if (argc < 5 || argc > 6) {
        fprintf(stderr, "Usage: %s #customers #tellers simulationTime averageServiceTime [--plot-mode]\n", argv[0]);
        fprintf(stderr, "Example: %s 100 4 60 2.3\n", argv[0]);
        return 1;
    }


    // Parse command line arguments
    char *endptr;
    long num_customers_long = strtol(argv[1], &endptr, 10);
    if (*endptr != '\0' || num_customers_long <= 0) {
        fprintf(stderr, "Error: Invalid number of customers. Must be a positive integer.\n");
        return 1;
    }
    long num_tellers_long = strtol(argv[2], &endptr, 10);
    if (*endptr != '\0' || num_tellers_long <= 0) {
        fprintf(stderr, "Error: Invalid number of tellers. Must be a positive integer.\n");
        return 1;
    }

    int num_customers = (int)num_customers_long;
    int num_tellers = (int)num_tellers_long;
    double simulation_time = strtod(argv[3], &endptr);
    if (*endptr != '\0' || simulation_time <= 0) {
        fprintf(stderr, "Error: Invalid simulation time. Must be a positive number.\n");
        return 1;
    }
    double avg_service_time = strtod(argv[4], &endptr);
    if (*endptr != '\0' || avg_service_time <= 0) {
        fprintf(stderr, "Error: Invalid average service time. Must be a positive number.\n");
        return 1;
    }

    // Check for plot mode flag
    int plot_mode = 0;
    if (argc == 6 && strcmp(argv[5], "--plot-mode") == 0) {
        plot_mode = 1;
    }

    // Seed the random number generator
    srand(time(NULL));

    SimConfig config = {
        .numCustomers = num_customers,
        .numTellers = num_tellers,
        .simulationTime = simulation_time,
        .averageServiceTime = avg_service_time
    };

    // --- Run Single Queue Simulation ---
    config.queueType = SINGLE_QUEUE;
    Simulation* sim_single = run_and_get_stats(&config);

    // Write data for gnuplot
    if (sim_single) {
        FILE *gnuplot_data = fopen("output/gplot.dat", "a");
        if (gnuplot_data) {
            double avg_time = (sim_single->customersServed > 0) ? sim_single->totalCustomerTimeInBank / sim_single->customersServed : 0;
            fprintf(gnuplot_data, "%d %.2f\n", sim_single->config.numTellers, avg_time);
            fclose(gnuplot_data);
        } else {
            fprintf(stderr, "Warning: Could not open output/gplot.dat for writing.\n");
        }
        cleanup_simulation(sim_single);
    }

    // If not in plot mode, also run the multiple-queue simulation for comparison
    if (!plot_mode) {
        config.queueType = MULTIPLE_QUEUES;
        Simulation* sim_multi = run_and_get_stats(&config);
        cleanup_simulation(sim_multi);
    }

    printf("\nSimulation complete.\n");

    return 0;
}
