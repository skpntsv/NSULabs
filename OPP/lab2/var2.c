#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <math.h>
#include <time.h>

#include <omp.h>

int threadsNum = 4;
#define N 10000			// Размер матрицы
double e = 0.000001;	// Е
double tau = 0.00001;	// Тау

double normOfVector(const double* x) {
	double norm = 0;
	for (size_t i = 0; i < N; ++i) {
		norm += x[i] * x[i];
	}

	return sqrt(norm);
}

double checking(const double* a, const double* b) {
	double norma = 0;
	norma = normOfVector(a) / normOfVector(b);

	return norma;
}

int main() {
	omp_set_num_threads(threadsNum);
	struct timespec start, end;
	double* A = (double*)calloc(N * N, sizeof(double));
	double*x = (double*)calloc(N, sizeof(double));
	double*b = (double*)calloc(N, sizeof(double));
	double*Ax = (double*)calloc(N, sizeof(double));

	for (size_t i = 0; i < N; ++i) {
		x[i] = 0;
		b[i] = N + 1;				// заполнение вектора b
		for (size_t j = 0; j < N; ++j) {
			if (i == j) {
				A[i * N + j] = 2.0;
			}
			else {
				A[i * N + j] = 1.0;
			}
		}
	}
	//----------------------------------------------------
	clock_gettime(CLOCK_MONOTONIC_RAW, &start);

	double check = 1;
	size_t i, j;
	while (check >= e) {
		#pragma omp parallel shared(A, b, x, Ax, e, tau, check) private(i, j)
		{
			#pragma omp for
			for (i = 0; i < N; ++i) {
				Ax[i] = 0;
				for (j = 0; j < N; ++j) {
					Ax[i] += A[i * N + j] * x[j];
				}
			}
			#pragma omp for
			for (size_t i = 0; i < N; ++i) {
				Ax[i] = Ax[i] - b[i];
			}
			#pragma omp for
			for (size_t i = 0; i < N; ++i) {
				Ax[i] *= tau;
			}
			#pragma omp for
			for (size_t i = 0; i < N; ++i) {
				x[i] = x[i] - Ax[i];
			}

			//checking
			#pragma omp for
			for (i = 0; i < N; ++i) {
				Ax[i] = 0;
				for (j = 0; j < N; ++j) {
					Ax[i] += A[i * N + j] * x[j];
				}
			}
			#pragma omp for
			for (size_t i = 0; i < N; ++i) {
				Ax[i] = Ax[i] - b[i];
			}
			check = checking(Ax, b);
		}

	}
	for (size_t i = 0; i < N; ++i) {
		printf("%lf", x[i]);
	}
	printf("\n");

	clock_gettime(CLOCK_MONOTONIC_RAW, &end);

	printf("Time taken: %f seconds\n", end.tv_sec - start.tv_sec + 0.000000001 * (end.tv_nsec - start.tv_nsec));

	free(A);
	free(x);
	free(b);
	free(Ax);

	return 0;
}