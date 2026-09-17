# Source and target definitions
SRCS = nlohmann_json.h vega_lite_plot.h vega_lite_plot.cpp q3b.cpp
TARGET = q3b.out

# Compilation rules
$(TARGET): $(SRCS)
	g++ -Wall -o $(TARGET) $(SRCS)

# Run
run:
	./$(TARGET)

# Clean up binaries
clean:
	rm -f $(TARGET) q3b.html
