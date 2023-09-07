#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <math.h>
#include <time.h>

size_t N = 20000;		// размер матрицы
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

void multMatrixs(float* C, float* A, float* B) {
	for (int i = 0; i < N; i++) {
		float* c = C + i * N;
		for (int j = 0; j < N; j++) {
			c[j] = 0;
		}
		for (int k = 0; k < N; k++) {
			const float* b = B + k * N;
			float a = A[i * N + k];
			for (int j = 0; j < N; j++) {
				c[j] += a * b[j];
			}
		}
	}
}

float* createMatrixB(float* A) {
	float* B = (float*)calloc(N * N, sizeof(float));

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
	A = (float*)calloc(N * N, sizeof(float));
	R = (float*)calloc(N * N, sizeof(float));
	C1 = (float*)calloc(N * N, sizeof(float));
	C2 = (float*)calloc(N * N, sizeof(float));
	C3 = (float*)calloc(N * N, sizeof(float));
	A_rev = (float*)calloc(N * N, sizeof(float));

	srand((float)time(NULL));
	for (size_t i = 0; i < N; i++) {
		for (size_t j = 0; j < N; j++) {
			//scanf("%f", &A[i * N + j]);
			A[i * N + j] = (float)rand();
		}
	}

	clock_gettime(CLOCK_MONOTONIC_RAW, &start);

	//--Вычисляем матрицу B----------
	B = createMatrixB(A);
	//-------------------------

	//-- Вычисляем матрицу R------------
	multMatrixs(C1, B, A);
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
	for (size_t i = 0; i < N; i++) {
		for (size_t j = 0; j < N; j++) {
			C1[i * N + j] = R[i * N + j];
			C2[i * N + j] = R[i * N + j];
		}
		C1[i * N + i] += 1;
	}
	for (size_t m = 0; m < M; m++) {
		multMatrixs(C3, R, C2);
		for (size_t i = 0; i < N; i++) {
			for (size_t j = 0; j < N; j++) {
				C2[i * N + j] = C3[i * N + j];
				C1[i * N + j] += C3[i * N + j];
			}
		}
	}
	// и умножаем результат на B
	multMatrixs(A_rev, C1, B);

	clock_gettime(CLOCK_MONOTONIC_RAW, &end);
		printf("Time taken: %lf sec.\n", end.tv_sec - start.tv_sec
		+ 0.000000001 * (end.tv_nsec - start.tv_nsec));

	//---------Проверка------------
	multMatrixs(C1, A, A_rev);
	float check = maxLine(C1) * maxColumn(C1);

	printf("Проверка матрицы на единичную:\nMaxSumCol(A * A^-1) * MaxSumLine(A * A^-1) = %f\n", check);
	

	// освобождаем память от всех матриц
	free(A);
	free(B);
	free(R);
	free(A_rev);
	free(C1);
	free(C2);
	free(C3);

	return 0;
}