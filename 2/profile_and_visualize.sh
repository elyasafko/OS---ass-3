#!/bin/bash

# Check if gprof2dot is installed
if ! command -v gprof2dot &> /dev/null
then
    echo "gprof2dot could not be found. Please install it using 'pip install gprof2dot'."
    exit
fi

# Check if dot (Graphviz) is installed
if ! command -v dot &> /dev/null
then
    echo "dot (Graphviz) could not be found. Please install it using 'sudo apt-get install graphviz'."
    exit
fi

# Compile all versions with profiling enabled
make clean
make CFLAGS="-std=c++17 -Wall -Wextra -pg"

# Function to generate random graph data
generate_random_data() 
{
    local vertices=$1
    local edges=$2
    echo "$vertices $edges"
    for ((i = 0; i < edges; i++)); do
        echo "$((RANDOM % vertices + 1)) $((RANDOM % vertices + 1))"
    done
}

# Parameters for random data
VERTICES=10000000
EDGES=50000000
DATA_FILE="random_data.txt"

# Generate random data if file does not exist
if [ ! -f "$DATA_FILE" ]; then
    generate_random_data $VERTICES $EDGES > "$DATA_FILE"
fi

# Function to profile and generate graph
profile_and_generate_graph() 
{
    local target=$1
    local profile_file=$2
    local dot_file=$3
    local png_file=$4

    echo "Profiling $target..."
    ./$target < "$DATA_FILE"

    if [ $? -eq 0 ]; then
        gprof $target gmon.out > $profile_file
        rm -f gmon.out

        # Generate dot file and png
        gprof2dot -f prof $profile_file -o $dot_file
        dot -Tpng $dot_file -o $png_file
        echo "Graph generated: $png_file"
    else
        echo "Execution of $target failed."
    fi
}

# Profile each version and generate graphs
profile_and_generate_graph "kosaraju_deque" "kosaraju_deque_profile.txt" "kosaraju_deque_profile.dot" "kosaraju_deque_profile.png"
profile_and_generate_graph "kosaraju_deque_matrix" "kosaraju_deque_matrix_profile.txt" "kosaraju_deque_matrix_profile.dot" "kosaraju_deque_matrix_profile.png"
profile_and_generate_graph "kosaraju_list" "kosaraju_list_profile.txt" "kosaraju_list_profile.dot" "kosaraju_list_profile.png"
profile_and_generate_graph "kosaraju_list_matrix" "kosaraju_list_matrix_profile.txt" "kosaraju_list_matrix_profile.dot" "kosaraju_list_matrix_profile.png"

echo "Profiling and graph generation complete. Check the profile files and graphs for details."
