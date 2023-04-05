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

    double* C = (double*)calloc(n1 * n3, sizeof(double));
	if (rank == 0) {
		double* A = (double*)calloc(n1 * n2, sizeof(double));
		double* B = (double*)calloc(n2 * n3, sizeof(double));

		init(A, B, rank);
	}
	double* A_local = (double*)calloc(N * N, sizeof(double));
	double* B_local = (double*)calloc(N * N, sizeof(double));
	double* C_local = (double*)calloc(N * N, sizeof(double));

	// создаем 2D решетку процессоров
	int sqrt_size = (int)sqrt(size);
    int dims[2] = {sqrt_size, sqrt_size};
    int periods[2] = {1, 1};
    //MPI_Comm cart_comm;
    //MPI_Cart_create(MPI_COMM_WORLD, 2, dims, periods, 0, &cart_comm);

    // распределение матриц A и B по процессам
    countA = n1 / p1;
    countB = n3 / p2;
    countC = countA * countB;


	free(A);
	free(B);
	free(C);

	MPI_Finalize();

	return 0;
}
