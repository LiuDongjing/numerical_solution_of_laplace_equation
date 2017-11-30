TARGET=laplace_equation
INCDIR= -I ./

FLAG :=-fopenmp
CPP := g++
FILES := main.cpp jacobi_serial.cpp

all: $(TARGET)
	

$(TARGET): $(FILES)
	$(CPP) $(FLAG) $(FILES) -o  $@

help:
	@echo "Makefile for TARGET."
	@echo "Please see COPYING for licensing information."
	@echo "Output should be: "$(TARGET)
	@echo "Usage: make [ help | all | run | clean ] " 
	@echo
 
run:
	./$(TARGET)
        
clean:
	rm -f $(TARGET)