#include <stdio.h>
#include <stdlib.h>

#define X 10
#define Y 10

int countNeighbors(const int* A, int x, int y, int rows, int cols) {
    int count = 0;
    for (int i = x - 1; i <= x + 1; i++) {
        for (int j = y - 1; j <= y + 1; j++) {
            int r = (i + rows) % rows;
            int c = (j + cols) % cols;
            count += A[r * cols + c];
        }
    }
    count -= A[x * cols + y];
    return count;
}

void updateTable(int* A, int rows, int cols) {
    int* B = (int*)calloc(rows * cols, sizeof(int));
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            int neighbors = countNeighbors(A, i, j, rows, cols);
            if (A[i * cols + j] == 1) {
                if (neighbors < 2 || neighbors > 3) {
                    B[i * cols + j] = 0;
                }
                else {
                    B[i * cols + j] = 1;
                }
            }
            else {
                if (neighbors == 3) {
                    B[i * cols + j] = 1;
                }
            }
        }
    }
    for (int i = 0; i < rows * cols; i++) {
        A[i] = B[i];
    }
    free(B);
}

void printTable(const int* A, int rows, int cols) {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            printf("%c ", A[i * cols + j] ? 'X' : '-');
        }
        printf("\n");
    }
    printf("\n");
}

void initTable(int* A) {
    A[1 * Y + 1] = 1;
    A[2 * Y + 2] = 1;
    A[3 * Y + 0] = 1;
    A[3 * Y + 1] = 1;
    A[3 * Y + 2] = 1;
}

int main(int argc, char* argv[]) {
    int* table = (int*)calloc(X * Y, sizeof(int));

    initTable(table);
    printTable(table, X, Y);

    for (int i = 0; i < 10; i++) {
        updateTable(table, X, Y);
        printTable(table, X, Y);
    }

    free(table);
    return 0;
}
