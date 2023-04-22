#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <mpi.h>

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

void multMatrixs(double* C, const double* A, const double* B, int A_block_size, int B_block_size, int n2) {
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

void initMatrixs(double* A, double* B, int rank, int n1, int n2, int n3) {
    srand(rank);
    for (size_t i = 0; i < n1 * n2; ++i) {
        A[i] = (double)rand() / RAND_MAX;
        //A[i] = i;
    }
    for (size_t i = 0; i < n2 * n3; ++i) {
        B[i] = (double)rand() / RAND_MAX;
        //B[i] = i;
    }
}

void transposeMatrix(double *A, int rows, int cols) {
	double *tmp = (double*)malloc(rows * cols * sizeof(double));
	memcpy(tmp, A, rows * cols * sizeof(double));

	for (int i = 0; i < rows; ++i) {
		for (int j = 0; j < cols; ++j) {
		  A[j * rows + i] = tmp[i * cols + j];
		}
	}

	free(tmp);
}

void scatterMatrix(const double* A, double** A_local, int rows, int cols, int p, int rank, int size, int coord, int *count, MPI_Comm comm) {
	(*A_local) = (double*)calloc(rows / p * cols, sizeof(double));
	if (coord == 0) {
		MPI_Scatter(A, rows / p * cols, MPI_DOUBLE, *A_local, rows / p * cols, MPI_DOUBLE, 0, comm);
	}
}

void gatherMatrix(const double *C_local, double *C, int A_block_size, int B_block_size, int n1, int n2, int n3, int size, MPI_Comm comm) {
    MPI_Datatype matrixColumnType;
    MPI_Datatype columnType;

    int dimsX = n1 / A_block_size;
    int dimsY = n3 / B_block_size;
    int *recvCounts = malloc(sizeof(int) * size);
    int *displs = malloc(sizeof(int) * size);

    MPI_Type_vector(A_block_size, B_block_size, n3, MPI_DOUBLE, &matrixColumnType);
    MPI_Type_commit(&matrixColumnType);

    MPI_Type_create_resized(matrixColumnType, 0, B_block_size * sizeof(double), &columnType);
    MPI_Type_commit(&columnType);

    for (int i = 0; i < dimsX; ++i)
        for (int j = 0; j < dimsY; ++j)
        {
            recvCounts[i * dimsY + j] = 1;
            displs[i * dimsY + j] = j + i * dimsY * A_block_size;
        }

    MPI_Gatherv(C_local, A_block_size * B_block_size, MPI_DOUBLE, C, recvCounts, displs, columnType, 0, comm);

    MPI_Type_free(&matrixColumnType);
    MPI_Type_free(&columnType);
    free(recvCounts);
    free(displs);
}

int main(int argc, char *argv[]) {
	if (argc != 4) {
		printf("You need add 3 parametrs: n1 n2 n3\n");
		exit(1);
	}
	int n1 = atoi(argv[1]), n2 = atoi(argv[2]), n3 = atoi(argv[3]);
    int rank, size;
    int coords[2], reorder = 1;
    int dims[2] = {0, 0};
    int periods[2] = {1, 1};
    int sizeY = 0, sizeX = 0;

	MPI_Comm comm2d; // коммуникатор 2d решетки
	MPI_Comm commRows;		// коммуникатор строк
    MPI_Comm commColumns;	// коммуникатор столбцов


    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    MPI_Dims_create(size, 2, dims); // Создание коммуникатора comm2d
	sizeX = dims[0];
	sizeY = dims[1];
    MPI_Cart_create(MPI_COMM_WORLD, 2, dims, periods, reorder, &comm2d);

    if ((n1 % sizeX != 0) || (n3 % sizeY != 0)) {
    	MPI_Finalize();
    	printf("Smthg wrong\n");
    	exit(1);
    }

	MPI_Comm_rank(comm2d, &rank);

	MPI_Cart_get(comm2d, 2, dims, periods, coords);// получение координат в двумерной решетке

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

		initMatrixs(A, B, rank, n1, n2, n3);
		transposeMatrix(B, n2, n3);

		//printMatrix(A, n1, n2);
	}

    // распределение матриц A и B по процессам
    int A_block_size, B_block_size;
    A_block_size = n1 / sizeX; //p1
    B_block_size = n3 / sizeY; //p2
    double* A_local = NULL;
	double* B_local = NULL;
	double* C_local = (double*)calloc(A_block_size * B_block_size, sizeof(double));


	double startTime = MPI_Wtime();

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
	multMatrixs(C_local, A_local, B_local, A_block_size, B_block_size, n3);

	// 6. Матрица C собирается из (x,y) плоскости.
	gatherMatrix(C_local, C, A_block_size, B_block_size, n1, n2, n3, size, comm2d);

	double finishTime = MPI_Wtime();

    if (rank == 0) {
    	printf("Parametrs:\nn1 = %d\nn2 = %d\nn3 = %d\n", n1, n2, n3);
    	//printMatrix(C, n1, n3);

    	printf("Time: %lf\n", finishTime - startTime);

		free(A);
		free(B);
		free(C);
	}

	free(A_local);
	free(B_local);
	free(C_local);

	MPI_Comm_free(&comm2d);
    MPI_Comm_free(&commRows);
    MPI_Comm_free(&commColumns);


	MPI_Finalize();

	return 0;
}
