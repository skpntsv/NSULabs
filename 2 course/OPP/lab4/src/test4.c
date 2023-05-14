#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define ROWS 1000
#define COLS 1000

#define MAX_ITERS 2500

void fillSizes(int *lines, int size, int cols){
    for (int i = 0; i < size; i++){
        lines[i] = 0;
    }
    for (int i = 0; i < cols; i++){
        lines[i % size]++;
    }
}

void initFirstTwoLines(char *field, int sizeX){
    // orig : (1,2) (2,3) (3,1) (3,2) (3,3)
    // matr : (0,1) (1,2) (2,0) (2,1) (2,2)
    // where first coord is a number of a line
    field[1] = 1;
    field[sizeX+2] = 1;
}

void initThirdLine(char *field, int sizeX){
    // orig : (1,2) (2,3) (3,1) (3,2) (3,3)
    // matr : (0,1) (1,2) (2,0) (2,1) (2,2)
    // where first coord is a number of a line
    field[0] = 1;
    field[1] = 1;
    field[2] = 1;

}


void initFirstWhole(char *field, int cols){
    // orig : (1,2) (2,3) (3,1) (3,2) (3,3)
    // matr : (0,1) (1,2) (2,0) (2,1) (2,2)
    // where first coord is a number of a line
    field[(cols * 0) + 1] = 1;
    field[(cols * 1) + 2] = 1;
    field[(cols * 2) + 0] = 1;
    field[(cols * 2) + 1] = 1;
    field[(cols * 2) + 2] = 1;

}

int cmpfields(char *currentField, char *oldfield, int elems){
    for (int j = 0; j < elems; j++){
        if (currentField[j] != oldfield[j]) return 0;
    }
    return 1;
}

void fillostanov(char *ostanov, char *currentField, char **oldFields, int iters, int fsX, int linesPerProc){
    for (int i = 0; i < iters; i++){
        char* oldfield = oldFields[i];
        ostanov[i] = cmpfields(&currentField[fsX],&oldfield[fsX],fsX*linesPerProc);
    }
}

int countNeighbours(char *field, int cx, int cy, int fsX){
    int res = 0;
    for (int dy = -1; dy < 2; dy++){
        for (int dx = -1; dx < 2; dx++){
            if (dx == dy && dy == 0){
                continue;
            }
            int px = cx + dx;
            int py = cy + dy;
            if (px < 0){
                px = fsX - 1;
            } else if (px >= fsX){
                px = 0;
            }
            res += field[py * fsX + px];
        }
    }
    return res;
}

void calc_first_line(char *currentField, char *newfield, int fsX){
    for (int i = 0; i < fsX; i++){
        int nbrs = countNeighbours(currentField,i,1,fsX);
        if (currentField[fsX + i] == 0){
            newfield[fsX + i] = (nbrs == 3) ? 1 : 0;
        } else if (currentField[fsX + i] == 1){
            newfield[fsX + i] = (nbrs == 3 || nbrs == 2) ? 1 : 0;
        }
    }
}

void calc_last_line(char *currentField, char *newfield, int fsX, int linesPerProc){
    for (int i = 0; i < fsX; i++){
        int nbrs = countNeighbours(currentField,i,linesPerProc,fsX);
        if (currentField[fsX * linesPerProc + i] == 0){
            newfield[fsX * linesPerProc + i] = (nbrs == 3) ? 1 : 0;
        } else if (currentField[fsX * linesPerProc + i] == 1){
            newfield[fsX * linesPerProc + i] = (nbrs == 3 || nbrs == 2) ? 1 : 0;
        }
    }
    
}


void calc_without_headtail(char *currentField, char *newfield, int fsX, int linesPerProc){
    // нам нужна 3я строка - 1ая строка - копия от предыдущего ядра, 2ая - "первая" у нашего ядра
    for (int y = 2; y < linesPerProc; y++){
        for (int x = 0; x < fsX; x++){
            int nbrs = countNeighbours(currentField, x, y, fsX);
            if (currentField[y*fsX + x] == 0){
                newfield[y*fsX + x] = (nbrs == 3) ? 1 : 0;
            } else if (currentField[y*fsX+x] == 1){
                newfield[y*fsX + x] = (nbrs == 3 || nbrs == 2) ? 1 : 0;
            }
        }
    }

}

void printField(char *field, int fsX, int lines) {
    for (int i = 0; i < lines; i++) {
        for (int j = 0; j < fsX; j++) {
            printf("%c ", field[i * fsX + j] ? '#' : '.');
        }
        printf("\n");
    }
    printf("\n");
}


int main(int argc, char **argv){
    int fsX;
    int fsY;
    if (argc < 2) {
        fsX = ROWS;
        fsY = COLS;
    } else {
        fsX = atoi(argv[1]);
        fsY = atoi(argv[2]);
    }
    MPI_Init(&argc,&argv);
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);
    MPI_Comm_size(MPI_COMM_WORLD,&size);

    // кол-во строк поля на ядро (процесс)
    int linesPerProc[size];
    fillSizes(linesPerProc, size, fsY);

    // массив указателей на старые поля с предыдущих итераций
    char *oldFields[MAX_ITERS];

    // выделяем памяти сколько требуется + 2 строчки - копии соседских верхних \ нижних
    char *currentField = calloc((linesPerProc[rank] + 2) * fsX, sizeof(char));


    if (linesPerProc[0] > 2){
        // все стартовые значения влезают на поле 1 ядра
        if (rank == 0){
            initFirstWhole(&currentField[fsX],fsX);
        }
    } else if (linesPerProc[0] <= 2) {
        // не влезают
        if (rank == 0){
            initFirstTwoLines(&currentField[fsX],fsX);

        }
        if (rank == 1){
            initThirdLine(&currentField[fsX],fsX);
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

    for (int i = 0; i < MAX_ITERS; i++){
        char *newfield = calloc((linesPerProc[rank] + 2) * fsX, sizeof(char));
        MPI_Request sreq1, sreq2, rreq3, rreq4, areq6;

        //1. Инициализировать отправку первой строки предыдущему ядру, используя
        //MPI_Isend. Запомнить параметр request для шага 8.
        MPI_Isend(&currentField[fsX], fsX,MPI_CHAR, prev, 10, MPI_COMM_WORLD, &sreq1);

        //2. Инициализировать отправку последней строки последующему ядру, используя
        //MPI_Isend. Запомнить параметр request для шага 11.
        MPI_Isend(&currentField[fsX*(linesPerProc[rank])], fsX, MPI_CHAR, next, 20, MPI_COMM_WORLD, &sreq2);
        
        //3. Инициировать получение от предыдущего ядра его последней строки,
        //используя MPI_Irecv. Запомнить параметр request для шага 9.
        MPI_Irecv(currentField, fsX, MPI_CHAR, prev, 20, MPI_COMM_WORLD, &rreq3);

        //4. Инициировать получение от последующего ядра его первой строки, используя
        //MPI_Irecv. Запомнить параметр request для шага 12.
        MPI_Irecv(&currentField[fsX*(linesPerProc[rank]+1)], fsX, MPI_CHAR, next, 10, MPI_COMM_WORLD, &rreq4);

        // 5. Вычислить вектор флагов останова.
        // вектор с i элементами
        // ostanov[i] = 1 <=> состояние на текущей итерации совпало с iой итерацией
        char ostanov[i];
        fillostanov(ostanov, currentField, oldFields, i, fsX, linesPerProc[rank]);
        // обмен остановами
        MPI_Iallreduce(MPI_IN_PLACE, ostanov, i, MPI_CHAR, MPI_LAND, MPI_COMM_WORLD, &areq6);

        // 7. Вычислить состояния клеток в строках, кроме первой и последней.
        calc_without_headtail(currentField, newfield, fsX, linesPerProc[rank]);

        MPI_Status status;
        // 8. Дождаться освобождения буфера отправки первой строки предыдущему ядру,
        //используя MPI_Wait и сохраненный на шаге 1 параметр request.
        MPI_Wait(&sreq1, &status);

        // 9. Дождаться получения от предыдущего ядра его последней строки, используя
        // MPI_Wait и сохраненный на шаге 3 параметр request.
        MPI_Wait(&rreq3,&status);

        //10. Вычислить состояния клеток в первой строке.
        calc_first_line(currentField,newfield,fsX);


        // 11, 12 - wait 
        MPI_Wait(&sreq2,&status);
        MPI_Wait(&rreq4,&status);

        //13. Вычислить состояния клеток в последней строке.
        calc_last_line(currentField, newfield, fsX, linesPerProc[rank]);

        // 14 - wait 
        MPI_Wait(&areq6, &status);

        //15 Сравнить вектора флагов останова, полученные от всех ядер. Если для какой-то итерации все флаги равны 1, завершить выполнение программы
        for (int j = 0; j < i; j++){
            if (ostanov[j] == 1){
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
        //printField(currentField, fsX, linesPerProc[rank]);
    }


    if (rank == 0){
        endtime = MPI_Wtime();
        printf("finished on %d iteration\n", numIteration);
        printf("Time taken: %f\n seconds\n", endtime - starttime);
    }

    // почистить все старые поля
    for (int i = 0; i < numIteration; i++){
        free(oldFields[i]);
    }
    free(currentField);
    MPI_Finalize();
    return 0;
}
