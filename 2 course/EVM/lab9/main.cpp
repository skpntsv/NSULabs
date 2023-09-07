#include <iostream>
#include <limits>
#include <climits>
#include <random>
#include <vector>
#include <algorithm>

#define L1 32*1024
#define L2 256*1024
#define L3 3*1024*1024

#define K 10

double traversal(volatile size_t* array, size_t size) {
	union ticks {
		unsigned long long t64;
		struct s32 {
			long th, tl;
		} t32;
	} start, end;

	//double cpu_Hz = 2400000000ULL; // for 2.4 GHz CPU
	volatile size_t k = 0;
	volatile size_t i = 0;
	double res = std::numeric_limits<double>::max();


	for (size_t j = 0; j < K; j++) {
		asm ("rdtsc\n":"=a"(start.t32.th),"=d"(start.t32.tl));

		for (i = 0, k = 0; i < size; i++) {
			k = array[k];
		}

		asm ("rdtsc\n":"=a"(end.t32.th),"=d"(end.t32.tl));
		res = std::min(res, (double)(end.t64-start.t64));
	}

	return res / size;
}

void count(size_t n) {
	volatile size_t *S;

	size_t BlockSize = L1 + L2 / sizeof(size_t);
	size_t Offset = 15 * 1024 * 1024 / sizeof(size_t);

	S = new size_t[Offset * n]; // Прямой обход


    //// Инициализация массивов///
    for (size_t i = 0; i < BlockSize / n; i++) {
    	for (size_t j = 0; j < n - 1; j++) {
    		S[j*(Offset) + i] = (j+1)*(Offset) + i;
    	}
    	S[(n - 1) * Offset + i] = (i + 1) % (BlockSize / n);
    }
    ///////////////////////////////////////
	std::cout << traversal(S, Offset * n) << std::endl;


	delete [] S;
}
 
int main() {
   	
	std::cout << std::fixed;
	std::cout.precision(0);


	std::cout << "Число фрагментов\tЧисло тактов" << std::endl;
    for (size_t i = 1; i <= 32; i++) {
    	//std::cout << i << "\t\t\t";
    	count(i);
	}

    return 0;
}