#pragma once

#include "IStrategy.h"

class DefectorStrategy : public IStrategy
{
public:

	Choice vote() override;
};