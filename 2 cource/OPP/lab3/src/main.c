#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

#include <mpi.h>

#define N 10
#define n1 6
#define n2 6
#define n3 6

#define p1 2
#define p2 2

void printMatrix(const double* A, int rows, int cols) {
	for (size_t i = 0; i < rows; i++) {
		for (size_t j = 0; j < cols; j++) {
			printf("%lf ", A[i * cols + j]);
		}
		printf("\n");
	}
	printf("\n");
}

void multMatrixs(const double* A, const double* x, double* Ax) {
	for (size_t i = 0; i < N; ++i) {
		Ax[i] = 0;
		for (size_t j = 0; j < N; ++j) {
			Ax[i] += A[i * N + j] * x[j];
		}
	}
}

void initMatrixs(double* A, double* B, int rank) {
	// заполняем блоки матриц случайными значениями
    srand(rank);
    for (size_t i = 0; i < n1 * n2; ++i) {
        //A[i] = (double)rand() / RAND_MAX;
        A[i] = i;
        //printf("%lf\n", A[i]);
    }
    for (size_t i = 0; i < n2 * n3; ++i) {
        //B[i] = (double)rand() / RAND_MAX;
        B[i] = i;
    }

/*    for (size_t i = 0; i < n1; i++) {
		for (size_t j = 0; j < n2; j++) {
			A[i * n2 + j] = 5;
		}
	}*/
}

void transposeMatrix(double *A, int rows, int cols) {
  double *tmp = (double*)malloc(rows * cols * sizeof(double));
  memcpy(tmp, A, rows * cols * sizeof(double));

  for (int i = 0; i < rows; i++) {
    for (int j = 0; j < cols; j++) {
      A[j * rows + i] = tmp[i * cols + j];
    }
  }

  free(tmp);
}

void scatterMatrix(double* A, double** A_local, int rows, int cols, int p, int rank, int size, int coord, int *count, MPI_Comm comm) {
    if (coord == 0) {
	    int* displs = (int*)malloc((size + 1) * sizeof(int));
	    int* scounts = (int*)malloc((size + 1) * sizeof(int));
	    int offset = 0, extra_cols = rows % p;

	    for (size_t i = 0; i < size; i++) {
	        displs[i] = offset;
	        scounts[i] = (rows / p);
	        
	        if (i < extra_cols) {
	            scounts[i]++;
	            if (i == rank) {
	                (*count)++;
	            }
	        }
	        
	        offset += scounts[i] * cols;
	        scounts[i] *= cols;
	    }

	    (*A_local) = (double*)calloc(scounts[rank], sizeof(double));
		//MPI_Scatterv(A, scounts, displs, MPI_DOUBLE, *A_local, scounts[rank], MPI_DOUBLE, 0, comm);
		free(displs);
    	free(scounts);
    	//MPI_Scatter(A, rows / p * cols, MPI_DOUBLE, *A_local, rows / p * cols, MPI_DOUBLE, 0, comm);
	}
}

void bcastLines(double** A, int rows, int cols, int sizeX, int sizeY, int rank) {
	int start = (rank - (rank % sizeY));
	MPI_Comm commLine;
	MPI_Comm_split(MPI_COMM_WORLD, start, rank, &commLine);
	if (sizeY > 1) {
		MPI_Bcast(*A, rows * cols, MPI_DOUBLE, 0, commLine);
	}
}


int main(int argc, char *argv[]) {
    int rank, size;
    int coords[2], reorder = 1;
    int dims[2] = {0, 0};
    int periods[2] = {1, 1};
    int size2d, rank2d, sizeY = 0, sizeX = 0, rankY = 0, rankX = 0;
	int prevy, prevx, nexty, nextx;
	int sub_dims[2] = {0, 0};
	MPI_Comm comm2d; // коммуникатор 2d решетки
	MPI_Comm commRows;		// коммуникатор строк
    MPI_Comm commColumns;	// коммуникатор столбцов

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    //double start = MPI_Wtime();

    MPI_Dims_create(size, 2, dims); // Создание коммуникатора comm2d
	sizeX = dims[0];
	sizeY = dims[1];
	printf("sizeY = %d - sizeX = %d\n", sizeY, sizeX);
    MPI_Cart_create(MPI_COMM_WORLD, 2, dims, periods, reorder, &comm2d);

	MPI_Comm_rank(comm2d, &rank);

	MPI_Cart_get(comm2d, 2, dims, periods, coords);// получение координат в двумерной решетке
  	rankX = coords[0]; rankY = coords[1];

  	// определение номеров соседей: prevy, nexty, prevx, nextx
	MPI_Cart_shift(comm2d, 0, 1, &prevy, &nexty);
	MPI_Cart_shift(comm2d, 1, 1, &prevx, &nextx);

/*	sub_dims[0] = 0;
    sub_dims[1] = 1;
    MPI_Cart_sub(comm2d, sub_dims, &commRows);

    sub_dims[0] = 1;
    sub_dims[1] = 0;
    MPI_Cart_sub(comm2d, sub_dims, &commColumns);*/

    // MPI_Comm_split разделит коммуникатор comm2d на непересекающиеся субкоммуникаторы
    MPI_Comm_split(comm2d, coords[1], coords[0], &commColumns);
    MPI_Comm_split(comm2d, coords[0], coords[1], &commRows);



    double *A = NULL;
    double *B = NULL;
    double* C = NULL;
    if (rank == 0) {
		A = (double*)calloc(n1 * n2, sizeof(double));
		B = (double*)calloc(n2 * n3, sizeof(double));
		C = (double*)calloc(n1 * n3, sizeof(double));

		initMatrixs(A, B, rank);
		//printMatrix(B, n2, n3);
		transposeMatrix(B, n2, n3);

		printMatrix(A, n1, n2);
	}

    // распределение матриц A и B по процессам
    int countA, countB;
    countA = n1 / sizeX; //p1
    countB = n3 / sizeY; //p2
    double* A_local = NULL;
	double* B_local = NULL;

	scatterMatrix(A, &A_local, n1, n2, sizeX, rank, size, coords[1], &countA, commColumns);
	printf("CountA = %d\n", countA);
	//MPI_Bcast(A_local, countA * n2, MPI_DOUBLE, 0, commRows);
	//scatterMatrix(B, &B_local, n2, n3, sizeY, rank, size, coords[0], &countB, commRows);
	//transposeMatrix(B_local, countB, n2);
	//MPI_Bcast(B_local, countB * n2, MPI_DOUBLE, 0, commColumns);

	// Выделяем память по матрицу C_local
	double* C_local = (double*)calloc(countA * countB, sizeof(double));

    //printf("rank = %d\n", rank);
    if (rank == 0) {
		free(A);
		free(B);
		free(C);
		//printMatrix(A, n1, n2);
		//printMatrix(B, n2, n3);
		//printf("%lf ", A[1]);
		//printMatrix(A_local, countA, n2);
		
		//printMatrix(B_local, n2, countB);
	}
	if (rank == 0) {
		printf("rank = %d\n", rank);

		//printMatrix(A_local, countA, n2);
		//printMatrix(B_local, n2, countB);
	}
	free(A_local);
	//free(B_local);
	free(C_local);

	MPI_Finalize();

	return 0;
}
