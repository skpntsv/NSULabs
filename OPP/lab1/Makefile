all: main

main: var2.c
	mpicc -o var2 var2.c -lm -Wall

run: var2
	mpirun -np 2 var2

clean:
	rm -f var2
