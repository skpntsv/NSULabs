#include <stdlib.h>
#include <stdio.h>

#include <mpi.h>

#define N 10
#define p1 2
#define p2 2

void multMatrixs(const double* A, const double* x, double* Ax) {
	for (size_t i = 0; i < N; ++i) {
		Ax[i] = 0;
		for (size_t j = 0; j < N; ++j) {
			Ax[i] += A[i * N + j] * x[j];
		}
	}
}

int main(int argc, char *argv[]) {
    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

	double* A = (double*)calloc(N * N, sizeof(double));
	double* B = (double*)calloc(N * N, sizeof(double));
	double* C = (double*)calloc(N * N, sizeof(double));	
	perror("smthg");

	// создаем 2D решетку процессоров
    int dims[2] = {p1, p2};
    int periods[2] = {1, 1};
    MPI_Comm cart_comm;
    MPI_Cart_create(MPI_COMM_WORLD, 2, dims, periods, 0, &cart_comm);


	free(A);
	free(B);
	free(C);

	MPI_Finalize();

	return 0;
}
