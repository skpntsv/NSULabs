#include "RandomStrategy.h"


Choice RandomatorStrategy::vote() {
	std::srand(std::time(nullptr));
	std::rand();

	if ((std::rand() % 2) == 1) {
		return Choice::D;
	}
	return Choice::C;
}