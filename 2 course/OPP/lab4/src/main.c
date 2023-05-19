#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define ROWS 250
#define COLS 250

#define MAX_ITERS 2500

void fillSizes(int* lines, int size, int rows) {
    for (int i = 0; i < size; i++) {
        lines[i] = 0;
    }
    for (int i = 0; i < rows; i++) {
        lines[i % size]++;
    }
}

void initFirstTwoLines(char* field, int sizeX) {
    field[1] = 1;
    field[sizeX + 2] = 1;
}

void initThirdLine(char* field, int sizeX) {
    field[0] = 1;
    field[1] = 1;
    field[2] = 1;
}


void initFirstWhole(char* field, int rows) {
    field[(rows * 0) + 1] = 1;
    field[(rows * 1) + 2] = 1;
    field[(rows * 2) + 0] = 1;
    field[(rows * 2) + 1] = 1;
    field[(rows * 2) + 2] = 1;

}

int compareVector(char* currentField, char* oldfield, int n) {
    for (int i = 0; i < n; i++) {
        if (currentField[i] != oldfield[i]) {
            return 0;
        }
    }
    return 1;
}

void fillStopFlags(char* stopFlags, char* currentField, char** oldFields, int iters, int cols, int linesPerProc) {
    for (int i = 0; i < iters; i++) {
        stopFlags[i] = compareVector(&currentField[cols], &oldFields[i][cols], cols * linesPerProc);
    }
}

int countNeighbours(char* field, int x, int y, int cols) {
    int res = 0;
    for (int dy = -1; dy < 2; dy++) {
        for (int dx = -1; dx < 2; dx++) {
            if (dx == dy && dy == 0) {
                continue;
            }
            int px = x + dx;
            int py = y + dy;
            if (px < 0) {
                px = cols - 1;
            } else if (px >= cols) {
                px = 0;
            }
            res += field[py * cols + px];
        }
    }
    return res;
}

void updateFirstLine(char* currentField, char* newfield, int cols) {
    for (int i = 0; i < cols; i++) {
        int nbrs = countNeighbours(currentField, i, 1, cols);
        if (currentField[cols + i] == 0) {
            newfield[cols + i] = (nbrs == 3) ? 1 : 0;
        } else if (currentField[cols + i] == 1) {
            newfield[cols + i] = (nbrs == 3 || nbrs == 2) ? 1 : 0;
        }
    }
}

void updateLastLine(char* currentField, char* newfield, int cols, int linesPerProc) {
    for (int i = 0; i < cols; i++) {
        int nbrs = countNeighbours(currentField, i, linesPerProc, cols);
        if (currentField[cols * linesPerProc + i] == 0) {
            newfield[cols * linesPerProc + i] = (nbrs == 3) ? 1 : 0;
        } else if (currentField[cols * linesPerProc + i] == 1) {
            newfield[cols * linesPerProc + i] = (nbrs == 3 || nbrs == 2) ? 1 : 0;
        }
    }

}


void updateWithoutFandL(char* currentField, char* newfield, int cols, int linesPerProc) {
    // нам нужна 3я строка - 1ая строка - копия от предыдущего ядра, 2ая - "первая" у нашего ядра
    for (int y = 2; y < linesPerProc; y++) {
        for (int x = 0; x < cols; x++) {
            int nbrs = countNeighbours(currentField, x, y, cols);
            if (currentField[y * cols + x] == 0) {
                newfield[y * cols + x] = (nbrs == 3) ? 1 : 0;
            } else if (currentField[y * cols + x] == 1) {
                newfield[y * cols + x] = (nbrs == 3 || nbrs == 2) ? 1 : 0;
            }
        }
    }

}

void printField(char* field, int cols, int lines) {
    for (int i = 0; i < lines; i++) {
        for (int j = 0; j < cols; j++) {
            printf("%c ", field[i * cols + j] ? '#' : '.');
        }
        printf("\n");
    }
    printf("\n");
}


int main(int argc, char** argv) {
    int cols;
    int rows;
    if (argc < 2) {
        cols = ROWS;
        rows = COLS;
    } else {
        rows = atoi(argv[1]);
        cols = atoi(argv[2]);
    }

    MPI_Init(&argc, &argv);
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // кол-во строк поля на ядро (процесс)
    int linesPerProc[size];
    fillSizes(linesPerProc, size, rows);

    // массив указателей на старые поля с предыдущих итераций
    char* oldFields[MAX_ITERS];

    // выделяем памяти сколько требуется + 2 строчки - копии соседских верхних \ нижних
    char* currentField = calloc((linesPerProc[rank] + 2) * cols, sizeof(char));


    if (linesPerProc[0] > 2) {
        // все стартовые значения влезают на поле 1 ядра
        if (rank == 0) {
            initFirstWhole(&currentField[cols], cols);
        }
    } else if (linesPerProc[0] <= 2) {
        // не влезают
        if (rank == 0) {
            initFirstTwoLines(&currentField[cols], cols);

        }
        if (rank == 1) {
            initThirdLine(&currentField[cols], cols);
        }
    }

    int prev = (rank + size - 1) % size;
    int next = (rank + 1) % size;

    int flag = 0;
    int numIteration = 0;

    double starttime, endtime;
    if (rank == 0) {
        starttime = MPI_Wtime();
    }

    for (numIteration = 0; numIteration < MAX_ITERS; numIteration++) {
        char* newfield = calloc((linesPerProc[rank] + 2) * cols, sizeof(char));
        MPI_Request sreq[2], rreq[2], areq;

        //1. Инициализировать отправку первой строки предыдущему ядру, используя
        //MPI_Isend. Запомнить параметр request для шага 8.
        MPI_Isend(&currentField[cols], cols, MPI_CHAR, prev, 10, MPI_COMM_WORLD, &sreq[0]);

        //2. Инициализировать отправку последней строки последующему ядру, используя
        //MPI_Isend. Запомнить параметр request для шага 11.
        MPI_Isend(&currentField[cols * (linesPerProc[rank])], cols, MPI_CHAR, next, 20, MPI_COMM_WORLD, &sreq[1]);

        //3. Инициировать получение от предыдущего ядра его последней строки,
        //используя MPI_Irecv. Запомнить параметр request для шага 9.
        MPI_Irecv(currentField, cols, MPI_CHAR, prev, 20, MPI_COMM_WORLD, &rreq[0]);

        //4. Инициировать получение от последующего ядра его первой строки, используя
        //MPI_Irecv. Запомнить параметр request для шага 12.
        MPI_Irecv(&currentField[cols * (linesPerProc[rank] + 1)], cols, MPI_CHAR, next, 10, MPI_COMM_WORLD, &rreq[1]);

        // 5. Вычислить вектор флагов останова.
        char* stopFlags = malloc((numIteration + 1) * sizeof(char));
        fillStopFlags(stopFlags, currentField, oldFields, numIteration, cols, linesPerProc[rank]);
        // 6. обмен остановами
        MPI_Iallreduce(MPI_IN_PLACE, stopFlags, numIteration, MPI_CHAR, MPI_LAND, MPI_COMM_WORLD, &areq);

        // 7. Вычислить состояния клеток в строках, кроме первой и последней.
        updateWithoutFandL(currentField, newfield, cols, linesPerProc[rank]);

        MPI_Status status;
        // 8. Дождаться освобождения буфера отправки первой строки предыдущему ядру,
        //используя MPI_Wait и сохраненный на шаге 1 параметр request.
        MPI_Wait(&sreq[0], &status);

        // 9. Дождаться получения от предыдущего ядра его последней строки, используя
        // MPI_Wait и сохраненный на шаге 3 параметр request.
        MPI_Wait(&rreq[0], &status);

        //10. Вычислить состояния клеток в первой строке.
        updateFirstLine(currentField, newfield, cols);


        // 11, 12 - wait 
        MPI_Wait(&sreq[1], &status);
        MPI_Wait(&rreq[1], &status);

        //13. Вычислить состояния клеток в последней строке.
        updateLastLine(currentField, newfield, cols, linesPerProc[rank]);

        // 14 - wait 
        MPI_Wait(&areq, &status);

        //15 Сравнить вектора флагов останова, полученные от всех ядер. Если для какой-то итерации все флаги равны 1, завершить выполнение программы
        for (int i = 0; i < numIteration; i++) {
            if (stopFlags[i] == 1) {
                flag = 1;
                break;
            }
        }
        free(stopFlags);
        if (flag == 1) {
            break;
        }

        oldFields[numIteration] = currentField;
        currentField = newfield;

        MPI_Barrier(MPI_COMM_WORLD);

        // Print field
        // printf("Iteration: %d\n", numIteration);
        // printField(currentField, cols, linesPerProc[rank]);
    }


    if (rank == 0) {
        endtime = MPI_Wtime();
        printf("finished on %d iteration\n", numIteration);
        printf("Time taken: %f\n seconds\n", endtime - starttime);
    }

    // почистить все старые поля
    for (int i = 0; i < numIteration; i++) {
        free(oldFields[i]);
    }
    free(currentField);

    MPI_Finalize();
    return 0;
}
