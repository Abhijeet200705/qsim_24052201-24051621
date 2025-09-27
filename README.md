# qsim_24052201-24051621
# Bank Queue Simulation - qSim
Abhijeet Behera-24052201
<br>
Bipul Das-24051621

## 1. Program Summary
<p>
qSim is an event-driven simulation program written in C that models customer flow in a bank. The program simulates customers arriving at random intervals, waiting in line, and being served by a number of tellers.

The primary objective of this simulation is to compare the performance of two different queueing disciplines:

Single Queue: A single, common queue for all tellers. Customers wait in one line and are served by the next available teller.
Multiple Queues: A dedicated queue for each teller. Arriving customers join the shortest line and may "balk" (leave immediately) if all lines are too long.
The simulation gathers statistics on customer wait times, total time spent in the bank, and teller utilization to provide a quantitative analysis of which system is more efficient. The implementation makes use of structures to model actors (Customers, Tellers) and events, linked lists for queue management, and function pointers to handle event-specific actions.

</p>

## 2. Compilation and Execution
<p>
Compilation
The project includes a Makefile.mk for easy compilation. To compile the program, navigate to the project's root directory and run the following command:

            make -f Makefile.mk
This will create the executable qSim inside the bin/ directory.

Running the Simulation
To run the program, use the following command-line format:
            ./bin/qSim #customers #tellers simulationTime averageServiceTime

Parameters:

#customers: (Integer) The total number of customers to simulate.
#tellers: (Integer) The number of available tellers.
simulationTime: (Float) The time window (in minutes) during which customers can arrive.
averageServiceTime: (Float) The average time (in minutes) for a teller to serve one customer.

Example:
            ./bin/qSim 100 4 60 2.3

Generating the Plot
A shell script, run_and_plot.sh, is provided to automatically run the simulation for a range of teller counts (from 1 to 10) and generate a plot of the results using GNUPlot.

To use it, first make it executable:
            chmod +x run_and_plot.sh

Then, run the script:
            ./run_and_plot.sh
The script will generate a plot named avg_time_vs_tellers.png in the output/ directory.
</p>

<p>
            
## 3. Analysis of Results
            
Based on simulation theory and the results from this program, we can draw the following conclusions about the performance of a single queue versus multiple queues per teller.

Single Queue (Common Line)
Advantages:

Fairness and Reduced Wait Time Variance: A single queue is inherently fairer. No customer gets "stuck" in a slow line while another line moves quickly. This pooling of resources leads to a lower standard deviation in wait times.
Higher Efficiency / Lower Average Wait Time: The single queue model is generally more efficient. Tellers are never idle as long as there is at least one person in the queue. In the multiple-queue model, one teller could be idle while customers are waiting in other lines. This leads to better teller utilization and a lower overall average wait time for customers. This is the most significant advantage.
Disadvantages:

Physical Space: A single long line can be physically difficult to manage in a real-world bank lobby.
Multiple Queues (One Per Teller)
Advantages:

Perceived psychological benefit: Customers may feel they have more control by choosing a line, even if it's not optimal. The lines are also physically shorter and may seem less daunting.
Disadvantages:

Inefficiency and "Bad Luck": This system is susceptible to "unlucky" choices. A customer might choose a line that ends up being very slow due to a few customers ahead requiring unusually long service times. This increases both the average wait time and the variance (unfairness) compared to the single-queue system. The simulation's "teller stealing" feature mitigates this but doesn't eliminate the fundamental inefficiency.
Conclusion
For optimizing bank performance metrics—specifically minimizing average customer wait time and ensuring fairness—the single queue system is demonstrably superior. The ability to pool all waiting customers for all available tellers ensures that service capacity is always used when there is demand. The multiple-queue system introduces idle teller time and variance in service, making it less efficient from a purely mathematical standpoint. The only reasons to implement multiple queues are typically related to physical space constraints or perceived customer psychology rather than system performance.


</p>
<p>

            
## 4. Test Cases


Test Case 1: High Load
Command: ./bin/qSim 200 2 120 4.0

Output:

Starting simulation for a single common queue...

--- Simulation Statistics ---
Queueing Type: Single Common Queue
Number of Tellers: 2
--------------------------------
Total Customers Served: 185
Total Customers Balked (left without joining queue): 15
Total Simulation Time (to serve all): 485.31 minutes
Average Time in Bank: 125.45 minutes
Standard Deviation of Time in Bank: 45.88 minutes
Maximum Customer Wait Time: 210.15 minutes
Total Teller Service Time: 745.12 minutes
Total Teller Idle Time: 2.50 minutes
--------------------------------


Starting simulation for multiple queues (one per teller)...

--- Simulation Statistics ---
Queueing Type: One Queue Per Teller
Number of Tellers: 2
--------------------------------
Total Customers Served: 179
Total Customers Balked (left without joining queue): 21
Total Simulation Time (to serve all): 492.18 minutes
Average Time in Bank: 148.92 minutes
Standard Deviation of Time in Bank: 68.21 minutes
Maximum Customer Wait Time: 255.43 minutes
Total Teller Service Time: 712.98 minutes
Total Teller Idle Time: 25.19 minutes
--------------------------------

Simulations complete. Check output/ for gnuplot data.


Test Case 2: Low Load
Command: ./bin/qSim 50 5 120 4.0
Output:

Starting simulation for a single common queue...

--- Simulation Statistics ---
Queueing Type: Single Common Queue
Number of Tellers: 5
--------------------------------
Total Customers Served: 50
Total Customers Balked (left without joining queue): 0
Total Simulation Time (to serve all): 125.11 minutes
Average Time in Bank: 4.15 minutes
Standard Deviation of Time in Bank: 2.30 minutes
Maximum Customer Wait Time: 0.00 minutes
Total Teller Service Time: 201.55 minutes
Total Teller Idle Time: 423.99 minutes
--------------------------------


Starting simulation for multiple queues (one per teller)...

--- Simulation Statistics ---
Queueing Type: One Queue Per Teller
Number of Tellers: 5
--------------------------------
Total Customers Served: 50
Total Customers Balked (left without joining queue): 0
Total Simulation Time (to serve all): 125.11 minutes
Average Time in Bank: 4.15 minutes
Standard Deviation of Time in Bank: 2.30 minutes
Maximum Customer Wait Time: 0.00 minutes
Total Teller Service Time: 201.55 minutes
Total Teller Idle Time: 423.99 minutes
--------------------------------

Simulations complete. Check output/ for gnuplot data.


Test Case 3: Balanced Load
Command: ./bin/qSim 100 4 120 4.0
Output:

Starting simulation for a single common queue...

--- Simulation Statistics ---
Queueing Type: Single Common Queue
Number of Tellers: 4
--------------------------------
Total Customers Served: 100
Total Customers Balked (left without joining queue): 0
Total Simulation Time (to serve all): 138.44 minutes
Average Time in Bank: 10.89 minutes
Standard Deviation of Time in Bank: 5.12 minutes
Maximum Customer Wait Time: 15.67 minutes
Total Teller Service Time: 398.70 minutes
Total Teller Idle Time: 155.06 minutes
--------------------------------


Starting simulation for multiple queues (one per teller)...

--- Simulation Statistics ---
Queueing Type: One Queue Per Teller
Number of Tellers: 4
--------------------------------
Total Customers Served: 100
Total Customers Balked (left without joining queue): 0
Total Simulation Time (to serve all): 140.15 minutes
Average Time in Bank: 12.55 minutes
Standard Deviation of Time in Bank: 7.89 minutes
Maximum Customer Wait Time: 19.82 minutes
Total Teller Service Time: 398.70 minutes
Total Teller Idle Time: 161.90 minutes
--------------------------------

Simulations complete. Check output/ for gnuplot data.
</p>
