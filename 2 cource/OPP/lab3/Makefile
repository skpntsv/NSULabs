CC=mpicc
CFLAGS=-Wall
LDFLAGS=-o
SOURCES=./src/main.c
EXECUTABLE=main
OUTPUT=test

all: $(EXECUTABLE)
		
$(EXECUTABLE): $(SOURCES) 
	$(CC) $(LDFLAGS) $(OUTPUT) $(SOURCES) $(CFLAGS)

run: $(OUTPUT)
	mpirun -np 2 $(OUTPUT)

clean:
	rm -f $(OUTPUT)
