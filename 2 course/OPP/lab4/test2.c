#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <mpi.h>

#define ARRAY_SIZE 8
#define MAX_ITERATIONS 10000

// Function to calculate the next state of a cell
int calculateNextState(int currentState, int liveNeighbors) {
    if (currentState == 1) {
        if (liveNeighbors < 2 || liveNeighbors > 3) {
            return 0; // Cell dies
        }
    } else {
        if (liveNeighbors == 3) {
            return 1; // Cell comes to life
        }
    }
    return currentState; // Cell state remains the same
}

// Function to count the number of live neighbors of a cell
int countLiveNeighbors(int* array, int x, int y) {
    int count = 0;
    for (int i = -1; i <= 1; i++) {
        for (int j = -1; j <= 1; j++) {
            int neighborX = (x + i + ARRAY_SIZE) % ARRAY_SIZE;
            int neighborY = (y + j + ARRAY_SIZE) % ARRAY_SIZE;
            int neighborIndex = neighborX * ARRAY_SIZE + neighborY;
            if (array[neighborIndex] == 1) {
                count++;
            }
        }
    }
    return count;
}

int main(int argc, char **argv)
{
    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int rowsPerProcess = ARRAY_SIZE / size;
    int remainder = ARRAY_SIZE % size;

    int *array = (int *)malloc((rowsPerProcess * ARRAY_SIZE) * sizeof(int));

    // Initialize array with zeros
    for (int i = 0; i < rowsPerProcess * ARRAY_SIZE; i++)
    {
        array[i] = 0;
    }

    // Set sailboat pattern cells to 1
    array[1 * ARRAY_SIZE + 2] = 1;
    array[2 * ARRAY_SIZE + 3] = 1;
    array[3 * ARRAY_SIZE + 1] = 1;
    array[3 * ARRAY_SIZE + 2] = 1;
    array[3 * ARRAY_SIZE + 3] = 1;

    // Create a copy of the array for storing the next states
    int *newArray = (int *)malloc((rowsPerProcess * ARRAY_SIZE) * sizeof(int));

    // Initialize MPI requests
    MPI_Request requests[4];
    MPI_Status statuses[4];

    // Main loop
    for (int iteration = 0; iteration < MAX_ITERATIONS; iteration++)
    {
        // Update boundary rows using circular array boundaries
        MPI_Isend(&array[0], ARRAY_SIZE, MPI_INT, (rank - 1 + size) % size, 0, MPI_COMM_WORLD, &requests[0]);
        MPI_Irecv(&array[rowsPerProcess * ARRAY_SIZE], ARRAY_SIZE, MPI_INT, (rank + 1) % size, 0, MPI_COMM_WORLD, &requests[1]);
        MPI_Isend(&array[(rowsPerProcess - 1) * ARRAY_SIZE], ARRAY_SIZE, MPI_INT, (rank + 1) % size, 0, MPI_COMM_WORLD, &requests[2]);
        MPI_Irecv(&array[0], ARRAY_SIZE, MPI_INT, (rank - 1 + size) % size, 0, MPI_COMM_WORLD, &requests[3]);

        // Calculate the next state for each cell
        for (int i = 1; i <= rowsPerProcess; i++)
        {
            for (int j = 0; j < ARRAY_SIZE; j++)
            {
                int x = (rank * rowsPerProcess) + i - 1;
                int y = j;
                int liveNeighbors = countLiveNeighbors(array, x, y);
                newArray[(i - 1) * ARRAY_SIZE + j] = calculateNextState(array[(i - 1) * ARRAY_SIZE + j], liveNeighbors);
            }
        }
        // Wait for communication to complete
        MPI_Waitall(4, requests, statuses);

        // Copy the new array into the original array
        for (int i = 1; i <= rowsPerProcess; i++)
        {
            for (int j = 0; j < ARRAY_SIZE; j++)
            {
                array[(i - 1) * ARRAY_SIZE + j] = newArray[(i - 1) * ARRAY_SIZE + j];
            }
        }

        // Check if the program should terminate
        int stopFlag = (array[0] == array[(rowsPerProcess - 1) * ARRAY_SIZE]) ? 1 : 0;
        int *stopFlags = (int *)malloc(size * sizeof(int));
        MPI_Allgather(&stopFlag, 1, MPI_INT, stopFlags, 1, MPI_INT, MPI_COMM_WORLD);

        bool shouldTerminate = true;
        for (int i = 0; i < size; i++)
        {
            if (stopFlags[i] == 0)
            {
                shouldTerminate = false;
                break;
            }
        }
        free(stopFlags);

        // Exit the loop if all processes agree to terminate
        if (shouldTerminate)
        {
            break;
        }
    }

    // Gather the final state of the array in the root process
    int *finalArray = NULL;
    if (rank == 0)
    {
        finalArray = (int *)malloc((ARRAY_SIZE * ARRAY_SIZE) * sizeof(int));
    }

    MPI_Gather(array, rowsPerProcess * ARRAY_SIZE, MPI_INT, finalArray, rowsPerProcess * ARRAY_SIZE, MPI_INT, 0, MPI_COMM_WORLD);

    // Print the final state of the array
    if (rank == 0)
    {
        printf("Final State:\n");
        for (int i = 0; i < ARRAY_SIZE; i++)
        {
            for (int j = 0; j < ARRAY_SIZE; j++)
            {
                printf("%d ", finalArray[i * ARRAY_SIZE + j]);
            }
            printf("\n");
        }
        free(finalArray);
    }

    //free(array);
    //free(newArray);
    MPI_Finalize();
    return 0;
}