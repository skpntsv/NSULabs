#include "CopyCatStrategy.h"

Choice CopycatStrategy::vote() {

	return choice_;
}

void CopycatStrategy::update(Choice first, Choice second) {
	if (first == Choice::D || second == Choice::D) {
		choice_ = Choice::D;
	}
	else {
		choice_ = Choice::C;
	}
}