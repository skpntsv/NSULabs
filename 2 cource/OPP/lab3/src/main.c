#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

#include <mpi.h>

#define N 10
#define n1 4
#define n2 4
#define n3 4

#define X 0
#define Y 1

void printMatrix(const double* A, int rows, int cols) {
	for (size_t i = 0; i < rows; ++i) {
		for (size_t j = 0; j < cols; ++j) {
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
		MPI_Scatter(A, rows / p * cols, MPI_DOUBLE, *A_local, rows / p * cols, MPI_DOUBLE, 0, comm);
	}
}

void gatherMatrix(const double *C_local, double *C, int A_block_size, int B_block_size, int size, MPI_Comm comm) {
    MPI_Datatype not_resized_recv_t;
    MPI_Datatype resized_recv_t;

    int dims_x = n1 / A_block_size;
    int dims_y = n3 / B_block_size;
    int *recv_counts = malloc(sizeof(int) * size);
    int *displs = malloc(sizeof(int) * size);

    MPI_Type_vector(A_block_size, B_block_size, n3, MPI_DOUBLE, &not_resized_recv_t);
    MPI_Type_commit(&not_resized_recv_t);

    MPI_Type_create_resized(not_resized_recv_t, 0, B_block_size * sizeof(double), &resized_recv_t);
    MPI_Type_commit(&resized_recv_t);

    for (int i = 0; i < dims_x; ++i)
        for (int j = 0; j < dims_y; ++j)
        {
            recv_counts[i * dims_y + j] = 1;
            displs[i * dims_y + j] = j + i * dims_y * A_block_size;
        }

    MPI_Gatherv(C_local, A_block_size * B_block_size, MPI_DOUBLE, C, recv_counts, displs, resized_recv_t, 0, comm);

    MPI_Type_free(&not_resized_recv_t);
    MPI_Type_free(&resized_recv_t);
    free(recv_counts);
    free(displs);
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
  	rankX = coords[X]; rankY = coords[Y];

    // MPI_Comm_split разделит коммуникатор comm2d на непересекающиеся субкоммуникаторы
    MPI_Comm_split(comm2d, coords[Y], coords[X], &commColumns);
    MPI_Comm_split(comm2d, coords[X], coords[Y], &commRows);

    double *A = NULL;
    double *B = NULL;
    double* C = NULL;
    if (rank == 0) {
		A = (double*)calloc(n1 * n2, sizeof(double));
		B = (double*)calloc(n2 * n3, sizeof(double));
		C = (double*)calloc(n1 * n3, sizeof(double));

		initMatrixs(A, B, rank);
		transposeMatrix(B, n2, n3);

		printMatrix(A, n1, n2);
	}

    // распределение матриц A и B по процессам
    int A_block_size, B_block_size;
    A_block_size = n1 / sizeX; //p1
    B_block_size = n3 / sizeY; //p2
    double* A_local = NULL;
	double* B_local = NULL;
	double* C_local = (double*)calloc(A_block_size * B_block_size, sizeof(double));


	// 1. Матрица А распределяется по горизонтальным полосам вдоль координаты (x,0).
	scatterMatrix(A, &A_local, n1, n2, sizeX, rank, size, coords[Y], &A_block_size, commColumns);

	// 2. Матрица B распределяется по вертикальным полосам вдоль координаты (0,y).
	scatterMatrix(B, &B_local, n3, n2, sizeY, rank, size, coords[X], &B_block_size, commRows);
	transposeMatrix(B_local, B_block_size, n2);

	// 3. Полосы А распространяются в измерении y
	MPI_Bcast(A_local, A_block_size * n2, MPI_DOUBLE, 0, commRows);

	// 4. Полосы B распространяются в измерении х
	MPI_Bcast(B_local, B_block_size * n2, MPI_DOUBLE, 0, commColumns);

	// 5. Каждый процесс вычисляет одну подматрицу произведения.
	multMatrixs(C_local, A_local, B_local, A_block_size, B_block_size);

	if (rank == 0) {
		//transposeMatrix(C, n1, n3);
	    printMatrix(C_local, A_block_size, B_block_size);
	}

	// 6. Матрица C собирается из (x,y) плоскости.
	gatherMatrix(C_local, C, A_block_size, B_block_size, size, comm2d);

    if (rank == 0) {
    	printMatrix(C, n1, n3);
		free(A);
		free(B);
		free(C);
	}

	free(A_local);
	free(B_local);
	free(C_local);

	MPI_Finalize();

	return 0;
}
