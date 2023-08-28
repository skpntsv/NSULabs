#include "game.h"


void Game::Start(const std::vector<std::string>& strategyName,
	const GameMode & mode,
	const int steps,
	const std::string & matrix_file) {

	std::cout << "Game have started\n" << std::endl;

	if (!matrix_file.empty()) {
		MakeMatrixOfGame(matrix_file);
	}
	
	for (size_t i = 0; i < strategyName.size(); ++i) {
		finalScores.insert(std::make_pair(strategyName.at(i), 0));
	}

	Factory factory;
	StrategyRegistor(strategyName, factory);
	switch (mode) {
		//DETAILED MODE////////////////////////
	case GameMode::detailed:
		for (size_t i = 0; i < 3; ++i) {
			strategies_[i] = factory.create(strategyName[i]);
		}
		while (cmd_ != "quit") {
			std::cout << std::endl;
			std::cout << "Print anything to continue or \"quit\" to finish game" << std::endl;
			std::cin >> cmd_;
			std::cout << std::endl;
			if (cmd_ == "quit") {
				break;
			}
			getVoting(strategies_);
			getTourScores();
			getFinalScores(strategyName[0], strategyName[1], strategyName[2]);
			std::cout << std::setw(20) << std::left << "Strategies names"
				<< std::setw(20) << "Choices"
				<< std::setw(20) << "Points for step"
				<< std::setw(20) << "Scores" << std::endl;
			for (size_t i = 0; i < 3; ++i)
			{
				std::cout << std::setw(20) << strategyName[i]
					<< std::setw(20) << choiceOfAStrategy_[votingResults[i]]
					<< std::setw(20) << scoresAtStep[i]
					<< std::setw(20) << finalScores[strategyName[i]] << std::endl;
			}
		}
		printFinalScores(strategyName);
		break;

		////////FASTMODE////////////////
	case GameMode::fast:
		FastGame(strategyName[0], strategyName[1], strategyName[2], factory, steps);
		break;

		///////TOURNIMENT///////////
	case GameMode::tournament:
		for (size_t i = 0; i < strategyName.size() - 1; ++i) {
			for (size_t j = 0; j < strategyName.size(); ++j) {
				if ((i != j) && (i + 1 != j) && (i - 1 != j)) {
					FastGame(strategyName[i], strategyName[i + 1], strategyName[j], factory, steps);
					getFinalScores(strategyName[i], strategyName[i + 1], strategyName[j]);
				}
			}
		}
		printFinalScores(strategyName);
		break;

	default:
		throw std::invalid_argument("Invalid name of gamemode");
		break;
	}
}

void Game::FastGame(const std::string & first, 
		const std::string & second, 
		const std::string & third, 
		Factory & factory, 
		const int steps) {
	strategies_[0] = factory.create(first);
	strategies_[1] = factory.create(second);
	strategies_[2] = factory.create(third);
	for (size_t i = 0; i < steps; ++i) {
		getVoting(strategies_);
		getTourScores();
	}
	printResultsOfFastgame(first, second, third);
}