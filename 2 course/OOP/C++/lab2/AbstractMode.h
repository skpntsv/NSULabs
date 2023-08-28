#include "IStrategy.h"
#include "parser.h"

#include "CooperatorStrategy.h"
#include "DefectorStrategy.h"
#include "RandomStrategy.h"
#include "CopyCatStrategy.h"
#include "GrudgerStrategy.h"
#include "SoftCopyCatStrategy.h"

#include "Factory.h"

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <memory>
#include <array>
#include <map>
#include <iterator>
#include <iomanip>

class Mode {
public:
	Mode() = default;

	virtual ~Mode() = default;

protected:
	std::array<Choice, 3> votingResults;
	std::array<int, 3> scoresAtStep = {0, 0, 0};
	std::map<std::string, int> finalScores;

	void StrategyRegistor(const std::vector<std::string>& strategyName, Factory & factory);
	void MakeMatrixOfGame(const std::string filename);

	void getVoting(const std::array<std::shared_ptr<IStrategy>, 3> & strategies);
	void getFinalScores(const std::string& first,
		const std::string& second,
		const std::string& third);
	void getTourScores();

	void printResultsOfFastgame(std::string fisrt, std::string second, std::string third);
	void printFinalScores(const std::vector<std::string>& strategyName);

	void strategyUpdate(const std::array<std::shared_ptr<IStrategy>, 3>& strategies);
private:
	std::array<int, 3> tourScores_ = { 0, 0, 0 };
	std::map<std::array<Choice, 3>, std::array<int, 3>> matrixOfGame_ = { {
		{{Choice::C, Choice::C, Choice::C}, {4, 4, 4}},
		{{Choice::C, Choice::C, Choice::D}, {3, 3, 9}},
		{{Choice::C, Choice::D ,Choice::C}, {3, 9, 3}},
		{{Choice::D, Choice::C, Choice::C}, {9, 3, 3}},
		{{Choice::C, Choice::D, Choice::D}, {0, 5, 5}},
		{{Choice::D, Choice::C, Choice::D}, {5, 0, 5}},
		{{Choice::D, Choice::D ,Choice::C}, {5, 5, 0}},
		{{Choice::D ,Choice::D, Choice::D}, {1, 1, 1}}
}};
	std::map<std::array<Choice, 3>, std::array<int, 3>> :: iterator mog_ = matrixOfGame_.begin();
};