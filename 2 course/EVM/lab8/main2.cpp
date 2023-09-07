#include <iostream>
#include <limits>
#include <climits>
#include <random>
#include <vector>
#include <algorithm>

#define K 10

double traversal(volatile size_t* array, size_t size) {
	union ticks {
		unsigned long long t64;
		struct s32 {
			long th, tl;
		} t32;
	} start, end;

	//double cpu_Hz = 2400000000ULL; // for 2.4 GHz CPU
	size_t k = 0;
	size_t i = 0;
	double res = std::numeric_limits<double>::max();

	// прогрев 
	for(i = 0, k = 0; i < size * K; i++) {
		k = array[k];
	}

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

void count(size_t size) {

	volatile size_t *S, *B, *R;
	S = new size_t[size]; // Прямой обход
    B = new size_t[size]; // Обратный обход
    R = new size_t[size]; // Рандомный обход


    //// Инициализация массивов///
    for (size_t i = 0; i < size - 1; i++) {
        S[i] = i + 1;
    }
    S[size-1] = 0;
    ///////////////////////////////////////
	std::cout << traversal(S, size) << "\t\t";


    for (size_t i = size - 1; i > 0; i--) {
        B[i] = i - 1;
    }
    B[0] = size - 1;
    //////////////////////////////
    std::cout << traversal(B, size) << "\t\t"; 


    std::vector<size_t> v;
    for (size_t i = 1; i < size; i++) {
    	v.push_back(i);
    }

    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(v.begin(), v.end(), g);

    size_t k = 0;
    for (size_t i = 1; i < size; i++) {
    	R[k] = v.back();
    	v.erase(v.end() - 1);
    	k = R[k];
    }
    R[k] = 0;
    ////////////////////////////
    std::cout << traversal(R, size) << std::endl;
	

	delete [] S;
	delete [] R;
	delete [] B; 
}
 
int main() {
   	
    std::vector<size_t> N {256, 512, 1024, 8388608, 24576000};
	std::cout << std::fixed;
	std::cout.precision(0);

	size_t tmp = 0;
	std::cout << "\tПрямой обход\tОбратный обход\tСлучайный обход" << std::endl;
    for (size_t i = 128; i <= N.at(3) / 2; i *= 2) {
    	tmp = i * 8 / 1024;
    	if (tmp < 1024) {
    		std::cout << tmp << "KB\t";
    	}
    	else {
    		std::cout << tmp / 1024 << "MB\t";
    	}
		count(i);
	}

    return 0;
}