#!/bin/bash

# Always run make to ensure the binary is up-to-date.
# make is smart and will only recompile what's necessary.
echo "Compiling project..."
make -f Makefile.mk
if [ ! -f "bin/qSim" ]; then
    echo "Compilation failed. Exiting."
    exit 1
fi

# Ensure output directory exists
mkdir -p output

# Clean previous gnuplot data
> output/gplot.dat

# --- Simulation Parameters (with defaults) ---
if [ "$#" -ne 3 ]; then
    echo "Usage: $0 [#customers] [simulationTime] [averageServiceTime]"
    echo "Using default values: 500 customers, 480 min sim time, 3.5 min service time"
    CUSTOMERS=500
    SIM_TIME=480
    AVG_SERVICE_TIME=3.5
else
    CUSTOMERS=$1
    SIM_TIME=$2
    AVG_SERVICE_TIME=$3
fi

echo "Running simulations to generate gnuplot data..."

# Loop through different numbers of tellers (e.g., from 1 to 10)
for tellers in {1..10}
do
    echo "  - Running with $tellers tellers..."
    ./bin/qSim $CUSTOMERS $tellers $SIM_TIME $AVG_SERVICE_TIME --plot-mode > /dev/null # Hide simulation output
done

echo "Data generation complete. Data is in output/gplot.dat"

# --- Plotting with Gnuplot ---
echo "Generating plot with Gnuplot..."
if ! command -v gnuplot &> /dev/null
then
    echo "Error: gnuplot is not installed. Please install it to generate the plot."
    exit 1
fi

gnuplot -e "set terminal pngcairo size 800,600; \
            set output 'output/avg_time_vs_tellers.png'; \
            set title 'Average Time in Bank vs. Number of Tellers'; \
            set xlabel 'Number of Tellers'; \
            set ylabel 'Average Time in Bank (minutes)'; \
            set grid; \
            set key top right; \
            plot 'output/gplot.dat' using 1:2 with linespoints title 'Single Queue' pt 7"

echo "Plot saved to output/avg_time_vs_tellers.png"