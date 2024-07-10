# Compiler
CXX = g++

# Compiler Flags
CXXFLAGS = -std=c++11 -Wall -pthread

# Source Files
SRCS = server.cpp proactor.cpp

# Header Files
HDRS = proactor.hpp

# Object Files
OBJS = $(SRCS:.cpp=.o)

# Executable Name
EXEC = server

# Default Target
all: $(EXEC)

# Link object files to create executable
$(EXEC): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Compile source files to object files
%.o: %.cpp $(HDRS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean target
clean:
	rm -f $(OBJS) $(EXEC)

# Phony targets
.PHONY: all clean
