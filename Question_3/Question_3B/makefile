# Source and target definitions
SRCS = q3b.cpp
TARGET = q3b.exe
CXXFLAGS = -Wall -g -std=c++17

.PHONY: all run valgrind clean

all: $(TARGET)

$(TARGET): $(SRCS)
	g++ $(CXXFLAGS) $(SRCS) -o $(TARGET)

# Run
run: $(TARGET)
	./$(TARGET)

# Valgrind
valgrind: $(TARGET)
	valgrind ./$(TARGET)

# Clean up binaries
clean:
	rm -f $(TARGET)