# Compiler
CC = g++

# Compiler flags
CFLAGS = -std=c++17 -Wall -Wextra -pg

# Targets
DEQUE_TARGET = kosaraju_deque
DEQUE_MATRIX_TARGET = kosaraju_deque_matrix
LIST_TARGET = kosaraju_list
LIST_MATRIX_TARGET = kosaraju_list_matrix

# Source files
DEQUE_SRCS = kosaraju_deque.cpp
DEQUE_MATRIX_SRCS = kosaraju_deque_matrix.cpp
LIST_SRCS = kosaraju_list.cpp
LIST_MATRIX_SRCS = kosaraju_list_matrix.cpp

# Object files
DEQUE_OBJS = $(DEQUE_SRCS:.cpp=.o)
DEQUE_MATRIX_OBJS = $(DEQUE_MATRIX_SRCS:.cpp=.o)
LIST_OBJS = $(LIST_SRCS:.cpp=.o)
LIST_MATRIX_OBJS = $(LIST_MATRIX_SRCS:.cpp=.o)

# Default target
all: $(DEQUE_TARGET) $(DEQUE_MATRIX_TARGET) $(LIST_TARGET) $(LIST_MATRIX_TARGET)

# Deque version
$(DEQUE_TARGET): $(DEQUE_OBJS)
	$(CC) $(CFLAGS) -o $@ $^

# Deque matrix version
$(DEQUE_MATRIX_TARGET): $(DEQUE_MATRIX_OBJS)
	$(CC) $(CFLAGS) -o $@ $^

# List version
$(LIST_TARGET): $(LIST_OBJS)
	$(CC) $(CFLAGS) -o $@ $^

# List matrix version
$(LIST_MATRIX_TARGET): $(LIST_MATRIX_OBJS)
	$(CC) $(CFLAGS) -o $@ $^

# Compile deque source files to object files
kosaraju_deque.o: kosaraju_deque.cpp
	$(CC) $(CFLAGS) -c $< -o $@

# Compile deque matrix source files to object files
kosaraju_deque_matrix.o: kosaraju_deque_matrix.cpp
	$(CC) $(CFLAGS) -c $< -o $@

# Compile list source files to object files
kosaraju_list.o: kosaraju_list.cpp
	$(CC) $(CFLAGS) -c $< -o $@

# Compile list matrix source files to object files
kosaraju_list_matrix.o: kosaraju_list_matrix.cpp
	$(CC) $(CFLAGS) -c $< -o $@

# Clean up build files
clean:
	rm -f $(DEQUE_TARGET) $(DEQUE_MATRIX_TARGET) $(LIST_TARGET) $(LIST_MATRIX_TARGET) $(DEQUE_OBJS) $(DEQUE_MATRIX_OBJS) $(LIST_OBJS) $(LIST_MATRIX_OBJS) *.txt *.png *.dot

# Run the deque version
run_deque: $(DEQUE_TARGET)
	./$(DEQUE_TARGET)

# Run the deque matrix version
run_deque_matrix: $(DEQUE_MATRIX_TARGET)
	./$(DEQUE_MATRIX_TARGET)

# Run the list version
run_list: $(LIST_TARGET)
	./$(LIST_TARGET)

# Run the list matrix version
run_list_matrix: $(LIST_MATRIX_TARGET)
	./$(LIST_MATRIX_TARGET)
