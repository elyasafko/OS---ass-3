# Compiler
CC = g++

# Compiler flags
CFLAGS = -std=c++17 -Wall -Wextra -pthread

# Targets
SERVER_TARGET = graph_server
CLIENT_TARGET = graph_client

# Source files
SERVER_SRCS = graph_server.cpp
CLIENT_SRCS = graph_client.cpp

# Object files
SERVER_OBJS = $(SERVER_SRCS:.cpp=.o)
CLIENT_OBJS = $(CLIENT_SRCS:.cpp=.o)

# Default target
all: $(SERVER_TARGET) $(CLIENT_TARGET)

# Build the server executable
$(SERVER_TARGET): $(SERVER_OBJS)
	$(CC) $(CFLAGS) -o $@ $^

# Build the client executable
$(CLIENT_TARGET): $(CLIENT_OBJS)
	$(CC) $(CFLAGS) -o $@ $^

# Compile source files to object files
%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

# Clean up build files
clean:
	rm -f $(SERVER_TARGET) $(CLIENT_TARGET) $(SERVER_OBJS) $(CLIENT_OBJS)

# Run the server
run_server: $(SERVER_TARGET)
	./$(SERVER_TARGET)

# Run the client
run_client: $(CLIENT_TARGET)
	./$(CLIENT_TARGET)

.PHONY: all clean run_server run_client
