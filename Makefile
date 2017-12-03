TARGET=laplace_equation
INCDIR= -I ./

INFO = 0
MODE = PARALLEL
CMD = mpi_run
FLAG :=-fopenmp
CXX := mpi_compile
FILES := main.cpp

ifeq ($(INFO),1)
    FLAG += -D DEBUG
endif

ifeq ($(MODE),PARALLEL)
    FLAG += -limf -D PARALLEL
    FILES += jacobi_parallel.cpp
else
    FILES += jacobi_serial.cpp
    CMD = 
    CXX = g++
endif

ifeq (run,$(firstword $(MAKECMDGOALS)))
  # use the rest as arguments for "run"
  RUN_ARGS := $(wordlist 2,$(words $(MAKECMDGOALS)),$(MAKECMDGOALS))
  # ...and turn them into do-nothing targets
  $(eval $(RUN_ARGS):;@:)
endif

all: 
	rm -f $(TARGET)
	$(CXX) $(FLAG) $(FILES) -o  $(TARGET)
	$(CMD) ./$(TARGET) $(RUN_ARGS)

$(TARGET): $(FILES)
	$(CXX) $(FLAG) $(FILES) -o  $@

help:
	@echo "Makefile for TARGET."
	@echo "Please see COPYING for licensing information."
	@echo "Output should be: "$(TARGET)
	@echo "Usage: make [ help | all | run | clean | $(TARGET) ] " 
	@echo

run:
	$(CMD) ./$(TARGET) $(RUN_ARGS)

clean:
	rm -f $(TARGET)
