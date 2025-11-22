# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -pedantic

# Target executable name
TARGET = display_app

# Source and object files
SRC = main.cpp DisplayState.cpp
OBJ = $(SRC:.cpp=.o)

# Default build target
all: $(TARGET)

# Link the executable
$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Compile source files into object files
%.o: %.cpp DisplayState.h FontMap.h
	$(CXX) $(CXXFLAGS) -c $<

# Clean up build files
clean:
	rm -f $(OBJ) $(TARGET)

# Phony targets
.PHONY: all clean
