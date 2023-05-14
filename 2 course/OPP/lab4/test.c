#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define ROWS 10
#define COLS 10

void initialize(int *grid, int rows, int cols) {
    // Инициализация массива нулями
    for (int i = 0; i < rows * cols; i++) {
        grid[i] = 0;
    }

    // Инициализация начального состояния клеток
    grid[COLS + 1] = 1;
    grid[2 * COLS + 2] = 1;
    grid[2 * COLS] = 1;
    grid[2 * COLS + 1] = 1;
    grid[2 * COLS + 2] = 1;
}

void printGrid(const int *grid, int rows, int cols) {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            printf("%d ", grid[i * cols + j]);
        }
        printf("\n");
    }
    printf("\n");
}

int countNeighbors(const int *grid, int rows, int cols, int x, int y) {
    int count = 0;
    for (int i = x - 1; i <= x + 1; i++) {
        for (int j = y - 1; j <= y + 1; j++) {
            int r = (i + rows) % rows;
            int c = (j + cols) % cols;
            count += grid[r * cols + c];
        }
    }
    count -= grid[x * cols + y];
    return count;
}

void updateGrid(int *grid, int rows, int cols) {
    int *newGrid = (int *)malloc(rows * cols * sizeof(int));

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            int index = i * cols + j;
            int neighbors = countNeighbors(grid, rows, cols, i, j);

            if (grid[index] == 1) {
                if (neighbors < 2 || neighbors > 3) {
                    newGrid[index] = 0;
                } else {
                    newGrid[index] = 1;
                }
            } else {
                if (neighbors == 3) {
                    newGrid[index] = 1;
                } else {
                    newGrid[index] = 0;
                }
            }
        }
    }

    for (int i = 0; i < rows * cols; i++) {
        grid[i] = newGrid[i];
    }

    free(newGrid);
}

int main(int argc, char *argv[]) {
    int rank, size;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int rowsPerProcess = ROWS / size;
    int remainder = ROWS % size;

    int startRow = rank * rowsPerProcess;
    int endRow = startRow + rowsPerProcess - 1;

    // Количество строк для каждого процесса
    if (rank < remainder) {
        startRow += rank;
        endRow += rank + 1;
    } else {
        startRow += remainder;
        endRow += remainder;
    }

    int colsPerProcess = COLS;

    // Выделение памяти для частичного массива
    int *partialGrid = (int *)malloc((rowsPerProcess + 2) * colsPerProcess * sizeof(int));

    initialize(partialGrid + colsPerProcess, rowsPerProcess, colsPerProcess);
    // Создание вектора флагов останова
    int stopFlag = 0;
    int *stopFlags = (int *)malloc(size * sizeof(int));

    // Создание запросов MPI для неблокирующих операций
    MPI_Request sendTopRequest, sendBottomRequest, recvTopRequest, recvBottomRequest, alltoallRequest;

    // Выполнение итераций обновления состояния клеточного массива
    int iterations = 0;
    while (1) {
        iterations++;

        // Инициализация отправки первой строки предыдущему процессу
        if (rank != 0) {
            MPI_Isend(partialGrid + colsPerProcess, colsPerProcess, MPI_INT, rank - 1, 0, MPI_COMM_WORLD, &sendTopRequest);
        }

        // Инициализация отправки последней строки последующему процессу
        if (rank != size - 1) {
            MPI_Isend(partialGrid + (rowsPerProcess * colsPerProcess), colsPerProcess, MPI_INT, rank + 1, 0, MPI_COMM_WORLD, &sendBottomRequest);
        }

        // Инициализация получения последней строки от предыдущего процесса
        if (rank != 0) {
            MPI_Irecv(partialGrid, colsPerProcess, MPI_INT, rank - 1, 0, MPI_COMM_WORLD, &recvTopRequest);
        }

        // Инициализация получения первой строки от последующего процесса
        if (rank != size - 1) {
            MPI_Irecv(partialGrid + ((rowsPerProcess + 1) * colsPerProcess), colsPerProcess, MPI_INT, rank + 1, 0, MPI_COMM_WORLD, &recvBottomRequest);
        }

        // Вычисление вектора флагов останова
        int localStopFlag = 0;
        for (int i = 1; i <= rowsPerProcess; i++) {
            for (int j = 0; j < colsPerProcess; j++) {
                int index = i * colsPerProcess + j;
                int neighbors = countNeighbors(partialGrid, rowsPerProcess + 2, colsPerProcess, i, j);

                if (partialGrid[index] == 1) {
                    if (neighbors < 2 || neighbors > 3) {
                        partialGrid[index] = 0;
                    }
                } else {
                    if (neighbors == 3) {
                        partialGrid[index] = 1;
                    }
                }

                if (partialGrid[index] != 0) {
                    localStopFlag = 1; // Флаг останова устанавливается, если были внесены изменения в текущем процессе
                }
            }   
        }
         // Инициализация обмена векторами флагов останова со всеми процессами
        MPI_Ialltoall(&localStopFlag, 1, MPI_INT, stopFlags, 1, MPI_INT, MPI_COMM_WORLD, &alltoallRequest);

        // Дождаться освобождения буфера отправки первой строки предыдущему процессу
        if (rank != 0) {
            MPI_Wait(&sendTopRequest, MPI_STATUS_IGNORE);
        }

        // Дождаться получения последней строки от предыдущего процесса
        if (rank != 0) {
            MPI_Wait(&recvTopRequest, MPI_STATUS_IGNORE);
        }

        // Вычислить состояния клеток в первой строке
        for (int j = 0; j < colsPerProcess; j++) {
            int index = colsPerProcess + j;
            int neighbors = countNeighbors(partialGrid, rowsPerProcess + 2, colsPerProcess, 1, j);

            if (partialGrid[index] == 1) {
                if (neighbors < 2 || neighbors > 3) {
                    partialGrid[index] = 0;
                }
            } else {
                if (neighbors == 3) {
                    partialGrid[index] = 1;
                }
            }

            if (partialGrid[index] != 0) {
                localStopFlag = 1;
            }
        }

        // Дождаться освобождения буфера отправки последней строки последующему процессу
        if (rank != size - 1) {
            MPI_Wait(&sendBottomRequest, MPI_STATUS_IGNORE);
        }

        // Дождаться получения первой строки от последующего процесса
        if (rank != size - 1) {
            MPI_Wait(&recvBottomRequest, MPI_STATUS_IGNORE);
        }

        // Вычислить состояния клеток в последней строке
        for (int j = 0; j < colsPerProcess; j++) {
            int index = (rowsPerProcess + 1) * colsPerProcess + j;
            int neighbors = countNeighbors(partialGrid, rowsPerProcess + 2, colsPerProcess, rowsPerProcess, j);

            if (partialGrid[index] == 1) {
                if (neighbors < 2 || neighbors > 3) {
                    partialGrid[index] = 0;
                }
            } else {
                if (neighbors == 3) {
                    partialGrid[index] = 1;
                }
            }

            if (partialGrid[index] != 0) {
                localStopFlag = 1;
            }
        }

        // Дождаться завершения обмена векторами флагов останова со всеми процессами
        MPI_Wait(&alltoallRequest, MPI_STATUS_IGNORE);

        // Проверить флаги останова, полученные от всех процессов
        for (int i = 0; i < size; i++) {
            if (stopFlags[i] == 1) {
                stopFlag = 1;
                break;
            }
        }

        // Остановить выполнение программы, если все флаги равны 1
        int globalStopFlag;
        MPI_Allreduce(&stopFlag, &globalStopFlag, 1, MPI_INT, MPI_LAND, MPI_COMM_WORLD);
        if (globalStopFlag == 1) {
            break;
        }

        // Обновление состояния клеточного массива
        updateGrid(partialGrid + colsPerProcess, rowsPerProcess, colsPerProcess);

        // Вывод состояния клеточного массива на экран
        if (rank == 0) {
            printf("Iteration: %d\n", iterations);
            printGrid(partialGrid + colsPerProcess, rowsPerProcess, colsPerProcess);
        }
        if (iterations >= 5) {
            break;
        }
    }

    // Освобождение памяти
    free(partialGrid);
    free(stopFlags);

    MPI_Finalize();

    return 0;
}

