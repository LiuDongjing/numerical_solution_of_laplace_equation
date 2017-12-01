TARGET=laplace_equation
INCDIR= -I ./

MODE = PARALLEL
CMD = mpi_run
FLAG :=-fopenmp
CXX := mpi_compile
FILES := main.cpp
ifeq ($(MODE),PARALLEL)
    FLAG += -limf -D PARALLEL
    FILES += jacobi_parallel.cpp
else
    FILES += jacobi_serial.cpp
    CMD = 
    CXX = g++
endif

all: 
	rm -f $(TARGET)
	$(CXX) $(FLAG) $(FILES) -o  $(TARGET)
	$(CMD) ./$(TARGET)

$(TARGET): $(FILES)
	$(CXX) $(FLAG) $(FILES) -o  $@

help:
	@echo "Makefile for TARGET."
	@echo "Please see COPYING for licensing information."
	@echo "Output should be: "$(TARGET)
	@echo "Usage: make [ help | all | run | clean | $(TARGET) ] " 
	@echo

run:
	$(CMD) ./$(TARGET)

clean:
	rm -f $(TARGET)