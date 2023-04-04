#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <math.h>
#include <time.h>

#define N 10000			// Размер матрицы
double e = 0.000001;	// Е
double tau = 0.00001;	// Тау

void printMatrix(const double* A, int lin, int col) {
	for (size_t i = 0; i < lin; i++) {
		for (size_t j = 0; j < col; j++) {
			printf("%lf ", A[i * col + j]);
		}
		printf("\n");
	}
	printf("\n");
}

void multMatrixs(const double* A, const double* x, double* Ax) {
	size_t i, j;
	for (i = 0; i < N; ++i) {
		Ax[i] = 0;
		for (j = 0; j < N; ++j) {
			Ax[i] += A[i * N + j] * x[j];
		}
	}
}

void subVectors(const double* a, const double* b, double* c) {
	for (size_t i = 0; i < N; ++i) {
		c[i] = a[i] - b[i];
	}
}

void scalarPerVector(double* x, double num) {
	for (size_t i = 0; i < N; ++i) {
		x[i] *= num;
	}
}

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

void makeAxb(const double* A, const double* x, const double* b, double* tmp) {
	multMatrixs(A, x, tmp);
	subVectors(tmp, b, tmp);
}

int main() {
	struct timespec start, end;

	double* A = NULL;		// матрица NxN
	double* x = NULL;		// вектор X
	double* b = NULL;		// вектор B
	double* Ax = NULL;		// вектор Ax

	A = (double*)calloc(N * N, sizeof(double));
	x = (double*)calloc(N, sizeof(double));
	b = (double*)calloc(N, sizeof(double));
	Ax = (double*)calloc(N, sizeof(double));

	//----------------------------------------------------
	for (size_t i = 0; i < N; ++i) {
		x[i] = 0;
		b[i] = N + 1;				// заполнение вектора b
		for (size_t j = 0; j < N; ++j) {
			if (i == j) {
				A[i * N + j] = 2.0;
			}
			else {
				//scanf("%f", &A[i * N + j]);
				A[i * N + j] = 1.0;
			}
		}
	}
	//----------------------------------------------------
	clock_gettime(CLOCK_MONOTONIC_RAW, &start);

	double check = 1;
	while (check >= e) {
		makeAxb(A, x, b, Ax);
		scalarPerVector(Ax, tau);
		subVectors(x, Ax, x);

		makeAxb(A, x, b, Ax);
		check = checking(Ax, b);

	}

	double tmp = 0;
	for (size_t i = 0; i < N; ++i) {
		tmp += x[i];
		//printf("%lf", x[i]);
	}
	printf("%f\n", N / tmp);

	clock_gettime(CLOCK_MONOTONIC_RAW, &end);

	printf("Time taken: %f seconds\n", end.tv_sec - start.tv_sec + 0.000000001 * (end.tv_nsec - start.tv_nsec));

	free(A);
	free(x);
	free(b);
	free(Ax);

	return 0;
}