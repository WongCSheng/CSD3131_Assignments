# Compiler
CXX = g++

# Compiler flags
CXXFLAGS = -std=c++20 -O2 -Wall -Wextra

# Target executable
TARGET = Greedy_algo_Seam_Carving.exe

# Source files
SRCS = Greedy_algo_Seam_Carving.cpp

# Object files
OBJS = $(SRCS:.cpp=.o)

# Default target
all: $(TARGET)

# Run the executable
run: $(TARGET)
	./$(TARGET)

# Link the object files to create the executable
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Compile source files into object files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean up build artifacts
clean:
	rm -f $(OBJS) $(TARGET)