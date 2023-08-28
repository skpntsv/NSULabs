#include "AbstractMode.h"

#include <iostream>
#include <iomanip>
#include <vector>
#include <string>
#include <array>
#include <map>


class Game : Mode {
public:
	void Start(const std::vector<std::string> & strategyName, 
		const GameMode & mode, 
		const int steps,
		const std::string & matrix_file);

private:
	std::array<std::shared_ptr<IStrategy>, 3> strategies_;
	std::map<Choice, std::string> choiceOfAStrategy_ = {
		{Choice::D, "Defect"},
		{Choice::C, "Cooperat"}
	};
	std::string cmd_;	// need for tournament mode

	void FastGame(const std::string & first, 
		const std::string & second, 
		const std::string & third, 
		Factory & factory, 
		const int steps);
};