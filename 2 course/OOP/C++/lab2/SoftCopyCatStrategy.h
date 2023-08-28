#pragma once

#include "IStrategy.h"

class SoftCopycatStrategy : public IStrategy {
public:

	Choice vote() override;
	void update(Choice first, Choice second) override;
private:
	Choice choice_ = Choice::C;
};