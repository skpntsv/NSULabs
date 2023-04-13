#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

#include <mpi.h>

#define N 10
#define n1 6
#define n2 6
#define n3 6

void printMatrix(const double* A, int rows, int cols) {
	for (size_t i = 0; i < rows; i++) {
		for (size_t j = 0; j < cols; j++) {
			printf("%lf ", A[i * cols + j]);
		}
		printf("\n");
	}
	printf("\n");
}

void multMatrixs(double* C, double* A, double* B, int A_block_size, int B_block_size) {
    for (int i = 0; i < A_block_size; ++i) {
        for (int j = 0; j < B_block_size; ++j) {
            C[i * B_block_size + j] = 0;
        }
    } 

    for (int i = 0; i < A_block_size; ++i) {
        for (int j = 0; j < n2; ++j) {
            for (int k = 0; k < B_block_size; ++k) {
                C[i * B_block_size + k] += A[i * n2 + j] * B[j * B_block_size + k];
            }
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
    (*A_local) = (double*)calloc(rows / p * n2, sizeof(double));
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

	    //(*A_local) = (double*)calloc(scounts[rank], sizeof(double));
	    //printf("scounts[%d] = %d\n", rank, scounts[rank]);
		MPI_Scatterv(A, scounts, displs, MPI_DOUBLE, *A_local, scounts[rank], MPI_DOUBLE, 0, comm);
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
    int A_block_size, B_block_size;
    A_block_size = n1 / sizeX; //p1
    B_block_size = n3 / sizeY; //p2
    double* A_local = NULL;
	double* B_local = NULL;

	scatterMatrix(A, &A_local, n1, n2, sizeX, rank, size, coords[1], &A_block_size, commColumns);
	//printf("CountA = %d\n", A_block_size);
	//printf("CountB = %d\n", B_block_size);
	MPI_Bcast(A_local, A_block_size * n2, MPI_DOUBLE, 0, commRows);
	scatterMatrix(B, &B_local, n3, n2, sizeY, rank, size, coords[0], &B_block_size, commRows);
	transposeMatrix(B_local, B_block_size, n2);
	MPI_Bcast(B_local, B_block_size * n2, MPI_DOUBLE, 0, commColumns);

	// Выделяем память по матрицу C_local
	double* C_local = (double*)calloc(A_block_size * B_block_size, sizeof(double));
	transposeMatrix(B_local, n2, B_block_size);


	multMatrixs(C_local, A_local, B_local, A_block_size, B_block_size);

    //printf("rank = %d\n", rank);
    if (rank == 0) {
		free(A);
		free(B);
		free(C);
		//printMatrix(A, n1, n2);
		//printMatrix(B, n2, n3);
		//printf("%lf ", A[1]);
		//printMatrix(A_local, A_block_size, n2);
		
		//printMatrix(B_local, n2, B_block_size);
	}
	if (rank == 0) {
		printf("rank = %d\n", rank);

		//printMatrix(A_local, A_block_size, n2);
		//printMatrix(B_local, n2, B_block_size);
	}
	free(A_local);
	//free(B_local);
	free(C_local);

	MPI_Finalize();

	return 0;
}
