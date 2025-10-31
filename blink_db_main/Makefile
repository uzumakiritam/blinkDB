# Makefile for BLINK DB Part B

# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O3 -g
LDFLAGS = -pthread

# Directories
SRCDIR = .
BUILDDIR = build
BINDIR = bin

# Source files
SOURCES = storage_engine.cpp server.cpp resp_protocol.cpp main.cpp
OBJECTS = $(patsubst %.cpp,$(BUILDDIR)/%.o,$(SOURCES))

# Target executable
TARGET = $(BINDIR)/blink_db

# Default target
all: directories $(TARGET)

# Create necessary directories
directories:
	@mkdir -p $(BUILDDIR)
	@mkdir -p $(BINDIR)
	@mkdir -p ../result

# Build the target executable
$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

# Compile source files
$(BUILDDIR)/%.o: $(SRCDIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean build files
clean:
	rm -rf $(BUILDDIR)/*.o $(TARGET)

# Run the server
run: all
	$(TARGET)

# Benchmark targets for 10,000 requests
benchmark_10000_10:
	redis-benchmark -p 9001 -n 10000 -c 10 -t set,get  > ../result/result_10000_10.txt

benchmark_10000_100:
	redis-benchmark -p 9001 -n 10000 -c 100 -t set,get > ../result/result_10000_100.txt

benchmark_10000_1000:
	redis-benchmark -p 9001 -n 10000 -c 1000 -t set,get > ../result/result_10000_1000.txt

# Benchmark targets for 100,000 requests
benchmark_100000_10:
	redis-benchmark -p 9001 -n 100000 -c 10 -t set,get > ../result/result_100000_10.txt

benchmark_100000_100:
	redis-benchmark -p 9001 -n 100000 -c 100 -t set,get > ../result/result_100000_100.txt

benchmark_100000_1000:
	redis-benchmark -p 9001 -n 100000 -c 1000 -t set,get > ../result/result_100000_1000.txt

# Benchmark targets for 1,000,000 requests
benchmark_1000000_10:
	redis-benchmark -p 9001 -n 1000000 -c 10 -t set,get > ../result/result_1000000_10.txt

benchmark_1000000_100:
	redis-benchmark -p 9001 -n 1000000 -c 100 -t set,get > ../result/result_1000000_100.txt

benchmark_1000000_1000:
	redis-benchmark -p 9001 -n 1000000 -c 1000 -t set,get > ../result/result_1000000_1000.txt

# Run all benchmarks
benchmark: benchmark_10000_10 benchmark_10000_100 benchmark_10000_1000 benchmark_100000_10 benchmark_100000_100 benchmark_100000_1000 benchmark_1000000_10 benchmark_1000000_100 benchmark_1000000_1000

# Generate documentation using doxygen
docs:
	doxygen docs/Doxyfile

# Phony targets
.PHONY: all directories clean run docs benchmark benchmark_10000_10 benchmark_10000_100 benchmark_10000_1000 benchmark_100000_10 benchmark_100000_100 benchmark_100000_1000 benchmark_1000000_10 benchmark_1000000_100 benchmark_1000000_1000
