# Compiler
CC = g++

# Compiler flags
CFLAGS = -Wall -g

# Targets
STACK_TARGET = kosaraju_stack

# Source files
STACK_SRCS = kosaraju_stack.cpp

# Object files
STACK_OBJS = $(STACK_SRCS:.cpp=.o)

# Default target
all: $(STACK_TARGET) $(QUEUE_TARGET)

# Stack version
$(STACK_TARGET): $(STACK_OBJS)
	$(CC) $(CFLAGS) -o $@ $^

# Compile stack source files to object files
kosaraju_stack.o: kosaraju_stack.cpp
	$(CC) $(CFLAGS) -c $< -o $@


# Clean up build files
clean:
	rm -f $(STACK_TARGET) $(QUEUE_TARGET) $(STACK_OBJS) $(QUEUE_OBJS)

# Run the stack version
run_stack: $(STACK_TARGET)
	./$(STACK_TARGET)

