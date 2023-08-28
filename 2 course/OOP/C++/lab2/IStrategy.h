#pragma once


enum class Choice {
	D,  // defect
	C   // cooperate
};


class IStrategy {
public:
	virtual ~IStrategy() = default;

	virtual Choice vote() = 0;

	virtual void update(Choice choice1, Choice choice2) {}
};