#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <math.h>
#include <time.h>

#include <mpi.h>

#define N 10000			// Размер матрицы
double e = 0.000001;	// Е
double tau = 0.00001;	// Тау

void printMatrix(const double* A, int lin, int col, int rank) {
	printf("rank = %d\n", rank);
	for (size_t i = 0; i < lin; i++) {
		for (size_t j = 0; j < col; j++) {
			printf("%lf ", A[i * col + j]);
		}
		printf("\n");
	}
	printf("\n");
}


void multMatrixs(const double* A, const double* x, double* Ax, int sizeChunk, int rank, int size, double* tmpX2) {
	size_t i, j, k, curRank;
	for (curRank = 0; curRank < size; ++curRank) {
		if (rank == curRank) {
			for (k = 0; k < sizeChunk; ++k) {
				tmpX2[k] = x[k];
			}
		}
		MPI_Bcast(tmpX2, sizeChunk, MPI_DOUBLE, curRank, MPI_COMM_WORLD);
		for (i = 0; i < sizeChunk; ++i) {
			for (j = curRank * sizeChunk, k = 0; j < (curRank + 1) * (sizeChunk); ++j, ++k) {
				Ax[i] += A[i * N + j] * tmpX2[k];
			}
		}
	}
}

void subVectors(const double* a, const double* b, double* c, int sizeChunk) {
	for (size_t i = 0; i < sizeChunk; ++i) {
		c[i] = a[i] - b[i];
	}
}

void scalarPerVector(double* x, double num, int sizeChunk) {
	for (size_t i = 0; i < sizeChunk; ++i) {
		x[i] *= num;
	}
}

double normOfVector(const double* x, int sizeChunk) {
	double norm = 0;
	for (size_t i = 0; i < sizeChunk; ++i) {
		norm += x[i] * x[i];
	}
	if (sizeChunk != N) {
		double globalNorm = 0;
		MPI_Allreduce(&norm, &globalNorm, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
		return sqrt(globalNorm);
	}
	return sqrt(norm);
}

double checking(const double* a, const double* b, int sizeChunk) {
	double norma = 0;
	norma = normOfVector(a, sizeChunk) / normOfVector(b, sizeChunk);

	return norma;
}

void makeAxb(const double* A, const double* x, const double* b, double* tmp, int sizeChunk, int rank, int size, double* tmpX2) {
	multMatrixs(A, x, tmp, sizeChunk, rank, size, tmpX2);
	subVectors(tmp, b, tmp, sizeChunk);
}

int main(int argc, char** argv) {
	MPI_Init(&argc, &argv);
	struct timespec start, end;
	int size, rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int sizeChunk = N / size;
    if (rank == size - 1) {
    	if (N % size != 0) {
    		sizeChunk += N % size;
    	}
    }
 	
    printf("sizeChunk = %d\n", sizeChunk);

	double* x = NULL;		// вектор X
	double* b = NULL;		// вектор B
	double* Ax = NULL;		// вектор Ax

	double* A_local = NULL;
	double* tmpX = NULL;
	double* tmpX2 = NULL;
	double* tmpOst = NULL;

	x = (double*)calloc(sizeChunk, sizeof(double));
	b = (double*)calloc(sizeChunk, sizeof(double));
	Ax = (double*)calloc(sizeChunk, sizeof(double));
	tmpX = (double*)calloc(sizeChunk, sizeof(double));
	tmpX2 = (double*)calloc(sizeChunk, sizeof(double));
	tmpOst = (double*)calloc(N % size + 1, sizeof(double));

	//----------------------------------------------------
	for (size_t i = 0; i < sizeChunk; ++i) {
		x[i] = 0;
		b[i] = N + 1;				// заполнение вектора b
	}
	//----------------------------------------------------
	clock_gettime(CLOCK_MONOTONIC_RAW, &start);
	
	// разрезаем основную матрицу
	A_local = (double*)calloc(sizeChunk * N, sizeof(double));
	for (size_t i = 0; i < sizeChunk; ++i) {
		for (size_t j = 0; j < N; ++j) {
			A_local[i * N + j] = 1.0;
		}
	}
	
	for (size_t i = 0; i < sizeChunk; ++i) {
		A_local[i * N + (rank * N / size) + i] = 2.0;
	}

	double check = 1;
	while (check >= e) {
		makeAxb(A_local, x, b, Ax, sizeChunk, rank, size, tmpX2);
		scalarPerVector(Ax, tau, sizeChunk);

		subVectors(x, Ax, x, sizeChunk);

		makeAxb(A_local, x, b, Ax, sizeChunk, rank, size, tmpX2);

		check = checking(Ax, b, sizeChunk);
	}

	//if (rank == 0) {
		//double tmp = 0;
/*		for (size_t i = 0; i < sizeChunk; ++i) {
			//tmp += x[i];
			printf("%lf", x[i]);
		}*/
		//printf("%f\n", N / tmp);
	//}

	clock_gettime(CLOCK_MONOTONIC_RAW, &end);
	if (rank == 0) {
		printf("Time taken: %f seconds\n", end.tv_sec - start.tv_sec + 0.000000001 * (end.tv_nsec - start.tv_nsec));
	}

	free(x);
	free(b);
	free(Ax);
	free(A_local);
	free(tmpX);
	free(tmpOst);

	MPI_Finalize();

	return 0;
}