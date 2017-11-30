TARGET=laplace_equation
INCDIR= -I ./

MODE = PARALLEL
CMD = mpirun
FLAG :=-fopenmp
CXX := mpicxx
FILES := main.cpp
ifeq ($(MODE),PARALLEL)
    FILES += jacobi_parallel.cpp
else
    FILES += jacobi_serial.cpp
    CMD = 
    CXX = g++
endif

all: $(TARGET)
	

$(TARGET): $(FILES)
	$(CXX) $(FLAG) $(FILES) -o  $@

help:
	@echo "Makefile for TARGET."
	@echo "Please see COPYING for licensing information."
	@echo "Output should be: "$(TARGET)
	@echo "Usage: make [ help | all | run | clean ] " 
	@echo

run:
	$(CMD) ./$(TARGET)

clean:
	rm -f $(TARGET)