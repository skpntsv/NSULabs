CC=mpicc
CFLAGS=-lm -Wall
LDFLAGS=-o $@
SOURCES=./src/main.c
EXECUTABLE=execute

NP=2

all: $(EXECUTABLE)
		
$(EXECUTABLE): $(SOURCES) 
	$(CC) $(LDFLAGS) $(SOURCES) $(CFLAGS)

run: $(EXECUTABLE)
	mpirun -np $(NP) $(EXECUTABLE)

clean:
	rm -f $(EXECUTABLE)
