# CPU Simulator Makefile - Assignment IV
# Performance Optimization: Data Forwarding + Cache

CXX = g++
CXXFLAGS = -std=c++11 -Wall -g
TARGET = simulator
OBJS = simulator.o pipeline.o registers.o data_memory.o memory.o performance.o log_handler.o cache.o

# Default target
all: $(TARGET)

# Link all object files
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

# Compile simulator.cpp
simulator.o: simulator.cpp pipeline.h registers.h data_memory.h performance.h log_handler.h cache.h
	$(CXX) $(CXXFLAGS) -c simulator.cpp

# Compile pipeline.cpp
pipeline.o: pipeline.cpp pipeline.h registers.h data_memory.h performance.h log_handler.h cache.h
	$(CXX) $(CXXFLAGS) -c pipeline.cpp

# Compile registers.cpp
registers.o: registers.cpp registers.h
	$(CXX) $(CXXFLAGS) -c registers.cpp

# Compile data_memory.cpp
data_memory.o: data_memory.cpp data_memory.h
	$(CXX) $(CXXFLAGS) -c data_memory.cpp

# Compile memory.cpp (instruction memory)
memory.o: memory.cpp
	$(CXX) $(CXXFLAGS) -c memory.cpp

# Compile performance.cpp
performance.o: performance.cpp performance.h cache.h
	$(CXX) $(CXXFLAGS) -c performance.cpp

# Compile log_handler.cpp
log_handler.o: log_handler.cpp log_handler.h
	$(CXX) $(CXXFLAGS) -c log_handler.cpp

# Compile cache.cpp (Assignment IV Part B)
cache.o: cache.cpp cache.h data_memory.h log_handler.h
	$(CXX) $(CXXFLAGS) -c cache.cpp

# Clean build files
clean:
	rm -f $(OBJS) $(TARGET) simulator.exe *.o

# Run the simulator
run: $(TARGET)
	./$(TARGET)

# Rebuild from scratch
rebuild: clean all

.PHONY: all clean run rebuild
