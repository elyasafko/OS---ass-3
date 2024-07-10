# Compiler
CXX = g++
# Compiler flags
CXXFLAGS = -Wall -pthread
# Target executable name
TARGET = server

# Source files
SRCS = graph_server.cpp

# Object files
OBJS = $(SRCS:.cpp=.o)

# Default target
all: $(TARGET)

# Link the target executable
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Compile source files into object files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean up generated files
clean:
	rm -f $(TARGET) $(OBJS)

# Rebuild the project
rebuild: clean all

.PHONY: all clean rebuild
