CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O3
LDFLAGS = -pthread

all: repl

repl: repl.o storage_engine.o
	$(CXX) $(CXXFLAGS) -o repl repl.o storage_engine.o $(LDFLAGS)

repl.o: repl.cpp storage_engine.h
	$(CXX) $(CXXFLAGS) -c repl.cpp

storage_engine.o: storage_engine.cpp storage_engine.h
	$(CXX) $(CXXFLAGS) -c storage_engine.cpp

clean:
	rm -f *.o repl

.PHONY: all clean
