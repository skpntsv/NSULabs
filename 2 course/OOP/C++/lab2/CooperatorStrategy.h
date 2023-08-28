#pragma once

#include "IStrategy.h"

class CooperatorStrategy : public IStrategy {
public:

	Choice vote() override;
};