CC=mpicc
CFLAGS=-Wall
LDFLAGS=-o $@
SOURCES=./src/test4.c
EXECUTABLE=execute

NP=2

all: $(EXECUTABLE)
		
$(EXECUTABLE): $(SOURCES) 
	$(CC) $(LDFLAGS) $(SOURCES) $(CFLAGS)

run: $(EXECUTABLE)
	mpirun --oversubscribe -np $(NP) ./$(EXECUTABLE)

clean:
	rm -f $(EXECUTABLE)
	