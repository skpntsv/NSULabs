#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include <mpi.h>

#define N 10
#define n1 5
#define n2 5
#define n3 5

void multMatrixs(const double* A, const double* x, double* Ax) {
	for (size_t i = 0; i < N; ++i) {
		Ax[i] = 0;
		for (size_t j = 0; j < N; ++j) {
			Ax[i] += A[i * N + j] * x[j];
		}
	}
}

void init(double* A, double* B, int rank) {
	// заполняем блоки матриц случайными значениями
    srand(rank);
    for (int i = 0; i < n1 * n2; i++) {
        A[i] = (double)rand() / RAND_MAX;
    }
    for (int i = 0; i < n2 * n3; i++) {
        B[i] = (double)rand() / RAND_MAX;
    }
}

int main(int argc, char *argv[]) {
    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (size % 2 != 0) {
        if (rank == 0) {
            printf("The number of processes must be a perfect square\n");
        }
        MPI_Finalize();
        return 0;
    }

    double *A = NULL, *B = NULL;
    double* C = (double*)calloc(n1 * n3, sizeof(double));
	if (rank == 0) {
		A = (double*)calloc(n1 * n2, sizeof(double));
		B = (double*)calloc(n2 * n3, sizeof(double));

		init(A, B, rank);
	}

	// создаем 2D решетку процессоров
	int p1 = (int)sqrt(size), p2 = (int)sqrt(size);
	//int coords[2], reorder=1;
    int dims[2] = {p1, p2};
    int periods[2] = {1, 1};
    //MPI_Comm cart_comm;
    //MPI_Cart_create(MPI_COMM_WORLD, 2, dims, periods, 0, &cart_comm);
/*    int size2d, rank2d, sizey, sizex, ranky, rankx;
	int prevy, prevx, nexty, nextx;
	MPI_Comm comm2d; // коммуникатор
	MPI_Comm_size(MPI_COMM_WORLD, &size2d);
	// определение размеров решетки: dims
	MPI_Dims_create(size2d, 2, dims);
	sizey = dims[0]; sizex = dims[1];
	// создание коммуникатора: comm2d
	MPI_Cart_create(MPI_COMM_WORLD, 2, dims, periods, reorder, &comm2d);
	// получение своего номера в comm2d: rank
	MPI_Comm_rank(comm2d, &rank2d);
	// получение своих координат в двумерной решетке: coords
	MPI_Cart_get(comm2d, 2, dims, periods, coords);
	ranky = coords[0]; rankx = coords[1];
	// определение номеров соседей: prevy, nexty, prevx, nextx
	MPI_Cart_shift(comm2d, 0, 1, &prevy, &nexty);
	MPI_Cart_shift(comm2d, 1, 1, &prevx, &nextx);*/


    // распределение матриц A и B по процессам
    int countA = n1 / p1;
    int countB = n3 / p2;

    double* A_local = (double*)calloc(countA * p2, sizeof(double));
	double* B_local = (double*)calloc(p2 * countB, sizeof(double));
	double* C_local = (double*)calloc(countA * countB, sizeof(double));

    printf("rank = %d\n", rank);
    if (rank == 0) {
		free(A);
		free(B);
	}
	free(C);
	free(A_local);
	free(B_local);
	free(C_local);

	MPI_Finalize();

	return 0;
}
