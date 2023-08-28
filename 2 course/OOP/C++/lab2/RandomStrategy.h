#pragma once

#include <cstdlib>
#include <ctime>

#include "IStrategy.h"

class RandomatorStrategy : public IStrategy
{
public:

	Choice vote() override;
};