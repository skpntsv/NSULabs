#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define ROWS 100
#define COLS 100

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
    // orig : (1,2) (2,3) (3,1) (3,2) (3,3)
    // matr : (0,1) (1,2) (2,0) (2,1) (2,2)
    // where first coord is a number of a line
    field[1] = 1;
    field[sizeX + 2] = 1;
}

void initThirdLine(char* field, int sizeX) {
    // orig : (1,2) (2,3) (3,1) (3,2) (3,3)
    // matr : (0,1) (1,2) (2,0) (2,1) (2,2)
    // where first coord is a number of a line
    field[0] = 1;
    field[1] = 1;
    field[2] = 1;

}


void initFirstWhole(char* field, int rows) {
    // orig : (1,2) (2,3) (3,1) (3,2) (3,3)
    // matr : (0,1) (1,2) (2,0) (2,1) (2,2)
    // where first coord is a number of a line
    field[(rows * 0) + 1] = 1;
    field[(rows * 1) + 2] = 1;
    field[(rows * 2) + 0] = 1;
    field[(rows * 2) + 1] = 1;
    field[(rows * 2) + 2] = 1;

}

int cmpfields(char* currentField, char* oldfield, int elems) {
    for (int j = 0; j < elems; j++) {
        if (currentField[j] != oldfield[j]) {
            return 0;
        }
    }
    return 1;
}

void fillostanov(char* ostanov, char* currentField, char** oldFields, int iters, int cols, int linesPerProc) {
    for (int i = 0; i < iters; i++) {
        char* oldfield = oldFields[i];
        ostanov[i] = cmpfields(&currentField[cols], &oldfield[cols], cols * linesPerProc);
    }
}

int countNeighbours(char* field, int cx, int cy, int cols) {
    int res = 0;
    for (int dy = -1; dy < 2; dy++) {
        for (int dx = -1; dx < 2; dx++) {
            if (dx == dy && dy == 0) {
                continue;
            }
            int px = cx + dx;
            int py = cy + dy;
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

void calc_first_line(char* currentField, char* newfield, int cols) {
    for (int i = 0; i < cols; i++) {
        int nbrs = countNeighbours(currentField, i, 1, cols);
        if (currentField[cols + i] == 0) {
            newfield[cols + i] = (nbrs == 3) ? 1 : 0;
        } else if (currentField[cols + i] == 1) {
            newfield[cols + i] = (nbrs == 3 || nbrs == 2) ? 1 : 0;
        }
    }
}

void calc_last_line(char* currentField, char* newfield, int cols, int linesPerProc) {
    for (int i = 0; i < cols; i++) {
        int nbrs = countNeighbours(currentField, i, linesPerProc, cols);
        if (currentField[cols * linesPerProc + i] == 0) {
            newfield[cols * linesPerProc + i] = (nbrs == 3) ? 1 : 0;
        } else if (currentField[cols * linesPerProc + i] == 1) {
            newfield[cols * linesPerProc + i] = (nbrs == 3 || nbrs == 2) ? 1 : 0;
        }
    }

}


void calc_without_headtail(char* currentField, char* newfield, int cols, int linesPerProc) {
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

    int bflag = 0;
    int numIteration = 0;

    double starttime, endtime;
    if (rank == 0) {
        starttime = MPI_Wtime();
    }

    for (int i = 0; i < MAX_ITERS; i++) {
        char* newfield = calloc((linesPerProc[rank] + 2) * cols, sizeof(char));
        MPI_Request sreq1, sreq2, rreq3, rreq4, areq6;

        //1. Инициализировать отправку первой строки предыдущему ядру, используя
        //MPI_Isend. Запомнить параметр request для шага 8.
        MPI_Isend(&currentField[cols], cols, MPI_CHAR, prev, 10, MPI_COMM_WORLD, &sreq1);

        //2. Инициализировать отправку последней строки последующему ядру, используя
        //MPI_Isend. Запомнить параметр request для шага 11.
        MPI_Isend(&currentField[cols * (linesPerProc[rank])], cols, MPI_CHAR, next, 20, MPI_COMM_WORLD, &sreq2);

        //3. Инициировать получение от предыдущего ядра его последней строки,
        //используя MPI_Irecv. Запомнить параметр request для шага 9.
        MPI_Irecv(currentField, cols, MPI_CHAR, prev, 20, MPI_COMM_WORLD, &rreq3);

        //4. Инициировать получение от последующего ядра его первой строки, используя
        //MPI_Irecv. Запомнить параметр request для шага 12.
        MPI_Irecv(&currentField[cols * (linesPerProc[rank] + 1)], cols, MPI_CHAR, next, 10, MPI_COMM_WORLD, &rreq4);

        // 5. Вычислить вектор флагов останова.
        // вектор с i элементами
        // ostanov[i] = 1 <=> состояние на текущей итерации совпало с iой итерацией
        char ostanov[i];
        fillostanov(ostanov, currentField, oldFields, i, cols, linesPerProc[rank]);
        // обмен остановами
        MPI_Iallreduce(MPI_IN_PLACE, ostanov, i, MPI_CHAR, MPI_LAND, MPI_COMM_WORLD, &areq6);

        // 7. Вычислить состояния клеток в строках, кроме первой и последней.
        calc_without_headtail(currentField, newfield, cols, linesPerProc[rank]);

        MPI_Status status;
        // 8. Дождаться освобождения буфера отправки первой строки предыдущему ядру,
        //используя MPI_Wait и сохраненный на шаге 1 параметр request.
        MPI_Wait(&sreq1, &status);

        // 9. Дождаться получения от предыдущего ядра его последней строки, используя
        // MPI_Wait и сохраненный на шаге 3 параметр request.
        MPI_Wait(&rreq3, &status);

        //10. Вычислить состояния клеток в первой строке.
        calc_first_line(currentField, newfield, cols);


        // 11, 12 - wait 
        MPI_Wait(&sreq2, &status);
        MPI_Wait(&rreq4, &status);

        //13. Вычислить состояния клеток в последней строке.
        calc_last_line(currentField, newfield, cols, linesPerProc[rank]);

        // 14 - wait 
        MPI_Wait(&areq6, &status);

        //15 Сравнить вектора флагов останова, полученные от всех ядер. Если для какой-то итерации все флаги равны 1, завершить выполнение программы
        for (int j = 0; j < i; j++) {
            if (ostanov[j] == 1) {
                bflag = 1;
                numIteration = i;
                break;
            }
        }
        if (bflag == 1) {
            break;
        }

        oldFields[i] = currentField;
        currentField = newfield;

        MPI_Barrier(MPI_COMM_WORLD);

        // Print the field
        //printf("Iteration: %d\n", i);
        //printField(currentField, cols, linesPerProc[rank]);
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
