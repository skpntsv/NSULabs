#include "GrudgerStrategy.h"

Choice GrudgerStrategy::vote() {

	return choice_;
}

void GrudgerStrategy::update(Choice first, Choice second) {
	if (first == Choice::D || second == Choice::D) {
		choice_ = Choice::D;
	}
}