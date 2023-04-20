CC=mpicc
CFLAGS=-lm 
LDFLAGS=-o $@
SOURCES=./src/main.c
EXECUTABLE=execute
PARAMETRS=4 4 4

NP=2

all: $(EXECUTABLE)
		
$(EXECUTABLE): $(SOURCES) 
	$(CC) $(LDFLAGS) $(SOURCES) $(CFLAGS)

run: $(EXECUTABLE)
	mpirun --oversubscribe -np $(NP) $(EXECUTABLE) $(PARAMETRS)

clean:
	rm -f $(EXECUTABLE)
