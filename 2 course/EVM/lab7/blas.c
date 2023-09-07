#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <math.h>
#include <time.h>

#include <cblas.h>

size_t N = 2048;		// размер матрицы
size_t M = 10;		//кол-во итераций в цикле

void printMatrix(float* A) {
	for (size_t i = 0; i < N; i++) {
		for (size_t j = 0; j < N; j++) {
			printf("%f ", A[i * N + j]);
		}
		printf("\n");
	}
	printf("\n");
}

float maxLine(float* A) {
	float sum = 0;
	float max = 0;
	for (size_t i = 0; i < N; i++) {
		sum = 0;
		for (size_t j = 0; j < N; j++) {
			sum += fabs(A[i * N + j]);
		}
		if (sum > max) {
			max = sum;
		}
	}

	return max;
}

float maxColumn(float* A) {
	float sum = 0;
	float max = 0;
	for (size_t i = 0; i < N; i++) {
		sum = 0;
		for (size_t j = 0; j < N; j++) {
			sum += fabs(A[i * N + j]);
		}
		if (sum > max) {
			max = sum;
		}
	}

	return max;
}

float* createMatrixB(float* A, float* memory) {
	float* B = memory + 6 * N * N;

	float multOfMax = maxLine(A) * maxColumn(A);
	for (size_t i = 0; i < N; i++) {
		for (size_t j = 0; j < N; j++) {
			B[i * N + j] = A[j * N + i] / multOfMax;
		}
	}

	return B;
}

int main() {

	struct timespec start, end;
	
	// создаём все нужные нам матрицы
	float* A = NULL;		// исходная матрица
	float* B = NULL;		// B = AT / |A|_1 * |A|_inf
	float* R = NULL;		// R = I - BA
	float* A_rev = NULL;	// Обратная матрица исходной матрицы
	float* C1 = NULL;		// tmp1 - матрица
	float* C2 = NULL;		// tmp2 - матрица
	float* C3 = NULL;		// tmp3 - матрица
	float* memory = malloc(7 * N * N * sizeof(float));
	A = memory;
	R = memory + N * N;
	C1 = memory + 2 * N * N;
	C2 = memory + 3 * N * N;
	C3 = memory + 4 * N * N;
	A_rev = memory + 5 * N * N;

	srand((float)time(NULL));
	for (size_t i = 0; i < N; i++) {
		for (size_t j = 0; j < N; j++) {
			//scanf("%f", &A[i * N + j]);
			A[i * N + j] = (float)rand();
		}
	}

	clock_gettime(CLOCK_MONOTONIC_RAW, &start);

	//--Вычисляем матрицу B----------
	B = createMatrixB(A, memory);
	//-------------------------

	//-- Вычисляем матрицу R------------

	cblas_sgemm(CblasRowMajor,CblasNoTrans,CblasNoTrans, N, N, N, 1.0 , B, N, A, N, 0.0, C1, N);
	for (size_t i = 0; i < N; i++) {
		for (size_t j = 0; j < N; j++) {
			if (i == j) {
				R[i * N + j] = 1 - C1[i * N + j];
			}
			else {
				R[i * N + j] = -C1[i * N + j];
			}
		}
	}
	
	//---Вычисляем обратную матрицу------
	// сложили I + R
	// C1 - сумма
	// C2 - R в степени n
	cblas_scopy(N*N, R, 1, C1, 1);
	cblas_scopy(N*N, R, 1, C2, 1);
	for (size_t i = 0; i < N; i++) {
		C1[i * N + i] += 1;
	}

	for (size_t m = 0; m < M; m++) {
		/*multMatrixs(C3, R, C2);*/
		cblas_sgemm(CblasRowMajor,CblasNoTrans,CblasNoTrans, N, N, N, 1.0 , R, N, C2, N, 0.0, C3, N);

		cblas_scopy(N*N, C3, 1, C2, 1);
		cblas_saxpy(N*N, 1.0, C3, 1, C1, 1);
	}
	// и умножаем результат на B
	cblas_sgemm(CblasRowMajor,CblasNoTrans,CblasNoTrans, N, N, N, 1.0 , C1, N, B, N, 0.0, A_rev, N);

	clock_gettime(CLOCK_MONOTONIC_RAW, &end);
		printf("Time taken: %lf sec.\n", end.tv_sec - start.tv_sec
		+ 0.000000001 * (end.tv_nsec - start.tv_nsec));

	//---------Проверка------------
	cblas_sgemm(CblasRowMajor,CblasNoTrans,CblasNoTrans, N, N, N, 1.0 , A, N, A_rev, N, 0.0, C1, N);
	float check = maxLine(C1) * maxColumn(C1);
	printf("Проверка матрицы на единичную:\nMaxSumCol(A * A^-1) * MaxSumLine(A * A^-1) = %f\n", check);
	

	// освобождаем память от всех матриц
	free(memory);

	return 0;
}