#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

#include <mpi.h>

#define N 10
#define n1 5
#define n2 5
#define n3 5

#define p1 2
#define p2 2

void printMatrix(const double* A, int rows, int cols) {
	for (size_t i = 0; i < rows; i++) {
		for (size_t j = 0; j < cols; j++) {
			printf("%lf ", A[i * cols + j]);
		}
		printf("\n");
	}
	printf("\n");
}

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

void transpose(double *matrix, int rows, int cols) {
  double *temp = (double*)malloc(rows * cols * sizeof(double));
  memcpy(temp, matrix, rows * cols * sizeof(double));

  for (int i = 0; i < rows; i++) {
    for (int j = 0; j < cols; j++) {
      matrix[j * rows + i] = temp[i * cols + j];
    }
  }

  free(temp);
}

void scatter_matrix(double* A, double** A_local, int a, int b, int p, int rank, MPI_Comm comm, int *countA) {
    int size = p1;
    int* displs = (int*)malloc((size + 1) * sizeof(int));
    int* scounts = (int*)malloc((size + 1) * sizeof(int));
    int offset = 0, extra_cols = a % p;
    
    for (size_t i = 0; i < size; i++) {
        displs[i] = offset;
        scounts[i] = (a / p);
        
        if (i < extra_cols) {
            scounts[i]++;
            if (i == rank) {
                (*countA)++;
            }
        }
        
        offset += scounts[i] * b;
        scounts[i] *= b;
    }

    (*A_local) = (double*)calloc(scounts[rank], sizeof(double));
    MPI_Scatterv(A, scounts, displs, MPI_DOUBLE, *A_local, scounts[rank], MPI_DOUBLE, 0, comm);
    
    free(displs);
    free(scounts);
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
	//int p1 = (int)sqrt(size), p2 = (int)sqrt(size);
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
    int countA, countB;
    countA = n1 / p1;
    countB = n3 / p2;
/*    if (rank == size - 1) {
    	countA += n1 % p1;
    	countB += n3 % p2;
    }*/
    //double* A_local = (double*)calloc(countA * n2, sizeof(double));
    double* A_local = NULL;
	double* B_local = (double*)calloc(n2 * countB, sizeof(double));
	double* C_local = (double*)calloc(countA * countB, sizeof(double));

//     int* displs = (int*)malloc((size + 1) * sizeof(int));
//     int* scounts = (int*)malloc((size + 1) * sizeof(int));
//     int offset = 0, extra_cols = n1 % p1;
//     for (size_t i = 0; i < size; i++) {
// /*      displs[i] = offset;
//         if ((i == size - 1) && (n1 % p1 != 0)) {
//         	scounts[i] = ((n1 / p1) + (n1 % p1)) * n2;
//         }
//         else {
// 	        scounts[i] = countA * n2;
//         }
//         offset += scounts[i];*/
//         displs[i] = offset;
//         scounts[i] = (n1 / p1);
//         if (i < extra_cols) {
//         	scounts[i]++;
//         	if (i == rank) {
//         		countA++;
//         	}
//     	}
//     	offset += scounts[i] * n2;
//     	scounts[i] *= n2;
//     }
//     double* A_local = (double*)calloc(scounts[rank], sizeof(double));
// 	MPI_Scatterv(A, scounts, displs, MPI_DOUBLE, A_local, scounts[rank], MPI_DOUBLE, 0, MPI_COMM_WORLD);
	// offset = 0;
    // for (size_t i = 0; i < size; i++) {
    //     displs[i] = offset;
    //     if ((i == size - 1) && (n2 % p2 != 0)) {
    //     	scounts[i] = ((n2 / p2) + (n2 % p2)) * n3;
    //     }
    //     else {
	//         scounts[i] = countB * n3;
    //     }
    //     offset += scounts[i];
    // }
    // if (rank == 0) {
	// 	//printMatrix(B, n2, n3);
	//     transpose(B, n2, n3);
	//     //printMatrix(B, n3, n2);
	// }
	// MPI_Scatterv(B, scounts, displs, MPI_DOUBLE, B_local, scounts[rank], MPI_DOUBLE, 0, MPI_COMM_WORLD);
	// transpose(B_local, countB, n2);
	scatter_matrix(A, &A_local, n1, n2, p1, rank, MPI_COMM_WORLD, &countA);

    printf("rank = %d\n", rank);
    if (rank == 0) {
		free(A);
		free(B);
		printMatrix(A, n1, n2);
		//printMatrix(B, n2, n3);
		//printf("%lf ", A[1]);
		//printMatrix(A_local, countA, n2);
		//printMatrix(B_local, n2, countB);
	}
	if (rank == 1) {
		printMatrix(A_local, countA, n2);
		//printMatrix(B_local, n2, countB);
	}
	free(C);
	free(A_local);
	free(B_local);
	free(C_local);

	MPI_Finalize();

	return 0;
}
