# Makefile for lock performance testing

CXX = g++
CXXFLAGS = -g -O3 -std=c++11 -Wall -pthread
TARGET = lock
SRC = lock.cpp
MAXTHREADS=40
MAXTIME=30

lock: $(SRC)
	$(CXX) $(CXXFLAGS) $(SRC) -o lock

team:
	@echo "Mandar Deshpande"

part1: compile_part1
	@echo "Executing matmul for n = 1024"; 
	./matmul 1024
	@echo "Executing matmul for n = 2048"; 
	./matmul 2048
	@echo "Executing matmul for n = 4096"; 
	./matmul 4096
	@echo "Executing matmul for n = 8192"; 
	./matmul 8192

compile_part1: matmul.cu
	nvcc -O3 -o matmul matmul.cu -std=c++11

# Build with correctness check
lock_check: $(SRC)
	$(CXX) $(CXXFLAGS) $(SRC) -o lock_check -DCHECK_CORRECTNESS

# Running the program
# Use make run MAXTHREADS=4 MAXTIME=10 to run with 4 threads for 10 seconds
run: lock
	./lock $(MAXTHREADS) $(MAXTIME)

# Running the program with correctness check
# Use make run_check MAXTHREADS=4 MAXTIME=10 to run with correctness check
run_check: lock_check
	./lock_check $(MAXTHREADS) $(MAXTIME)

# Clean compiled binaries
clean:
	rm -f lock_check lock matmul

.PHONY: all check clean run run_check
