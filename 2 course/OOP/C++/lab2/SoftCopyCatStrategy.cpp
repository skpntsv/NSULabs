#include "SoftCopyCatStrategy.h"

Choice SoftCopycatStrategy::vote() {

	return choice_;
}

void SoftCopycatStrategy::update(Choice first, Choice second) {
	if (first == Choice::D && second == Choice::D) {
		choice_ = Choice::D;
	}
	else {
		choice_ = Choice::C;
	}
}