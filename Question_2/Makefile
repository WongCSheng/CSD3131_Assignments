# Makefile for building and running the Question 2B program
.DEFAULT_GOAL := all

# Compiler
CXX = g++

# Compiler flags
CXXFLAGS = -std=c++23 -Wall -Wextra -O2

# Target executable
TARGET = my_program

# Source and object files
SOURCES = Q2BShowcase.cpp
OBJECTS = $(SOURCES:.cpp=.o)

# Compile source files into object files
$(OBJECTS): $(SOURCES)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Link object files into the executable
$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJECTS)

# Default build target
all: $(TARGET)

# Run the program
run: $(TARGET)
	./$(TARGET)

# Clean up build artifacts
clean:
	rm -f $(OBJECTS) $(TARGET)

.PHONY: all run clean