# Compiler
CC = g++

# Compiler flags
CFLAGS = -std=c++17 -Wall -Wextra -pg

# Executable name
TARGET = kosaraju_deque

# Source files
SRCS = kosaraju_deque.cpp

# Object files
OBJS = $(SRCS:.cpp=.o)

# Default target
all: $(TARGET)

# Build the executable
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

# Compile source files to object files
%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

# Clean up build files
clean:
	rm -f $(TARGET) $(OBJS) gmon.out

# Run the executable
run: $(TARGET)
	./$(TARGET)

.PHONY: all clean run
