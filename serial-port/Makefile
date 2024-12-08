# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2

# Libraries
LIBS = -lconfig++ -lpthread

# Project files
TARGET = serial_example
SRCS = serial_example.cpp ../display-state/DisplayState.cpp 
OBJS = $(SRCS:.cpp=.o)

# Default target
all: $(TARGET)

# Build target
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS) $(LIBS)

# Compile source files into objects
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean up
clean:
	rm -f $(TARGET) $(OBJS)

# Phony targets
.PHONY: all clean
