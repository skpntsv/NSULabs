#include "AbstractMode.h"

void Mode::StrategyRegistor(const std::vector<std::string>& strategyName, Factory & factory) {
	for (size_t i = 0; i < strategyName.size(); ++i) {
		// тривиальные стратегии
		if (strategyName[i] == "cooperator") {
			factory.register_creator(strategyName[i], std::make_shared <Creator<CooperatorStrategy>>());
		}
		if (strategyName[i] == "defector") {
			factory.register_creator(strategyName[i], std::make_shared<Creator<DefectorStrategy>>());
		}
		if (strategyName[i] == "randomator") {
			factory.register_creator(strategyName[i], std::make_shared<Creator<RandomatorStrategy>>());
		}
		// нетривиальные стратегии
		if (strategyName[i] == "copycat") {
			factory.register_creator(strategyName[i], std::make_shared<Creator<CopycatStrategy>>());
		}
		if (strategyName[i] == "grudger") {
			factory.register_creator(strategyName[i], std::make_shared<Creator<GrudgerStrategy>>());
		}
		if (strategyName[i] == "softcopycat") {
			factory.register_creator(strategyName[i], std::make_shared<Creator<SoftCopycatStrategy>>());
		}
	}
}

void Mode::MakeMatrixOfGame(const std::string filename) {
	std::ifstream FILE(filename);
	if (!FILE.is_open()) {
		throw std::invalid_argument("file don't have found");
	}
	else {
		int row = 0;
		std::advance(mog_, 7);
		std::array<int, 3> buffer;
		while (!FILE.eof()) {
			if (row > 7) {
				throw std::invalid_argument("The matrix is bigger");
			}
			else {
				FILE >> buffer[0] >> buffer[1] >> buffer[2];
				mog_->second = std::move(buffer);

				++row;
				mog_--;
			}
		}
		if (row < 8) {
			throw std::invalid_argument("The matrix is smaller");
		}
	}
}

void Mode::getVoting(const std::array<std::shared_ptr<IStrategy>, 3>& strategies) {
	for (size_t i = 0; i < 3; ++i) {
		votingResults[i] = (strategies[i]->vote());
	}
	strategyUpdate(strategies);

}

void Mode::getTourScores() {
	mog_ = matrixOfGame_.find(votingResults);
	scoresAtStep = mog_->second;

	for (size_t i = 0; i < 3; ++i) {
		tourScores_[i] += scoresAtStep[i];
	}

}

void Mode::getFinalScores(const std::string & first,
		const std::string & second,
		const std::string & third) {

	finalScores[first] += tourScores_[0];
	finalScores[second] += tourScores_[1];
	finalScores[third] += tourScores_[2];

	std::fill(tourScores_.begin(), tourScores_.end(), 0);

}

void Mode::strategyUpdate(const std::array<std::shared_ptr<IStrategy>, 3>& strategies) {
	strategies[0]->update(votingResults[1], votingResults[2]);
	strategies[1]->update(votingResults[0], votingResults[2]);
	strategies[2]->update(votingResults[0], votingResults[1]);
}

void Mode::printResultsOfFastgame(std::string first, std::string second, std::string third) {
	std::cout << std::setw(10) << std::left << first << ": "
		<< tourScores_[0] << " scores" << std::endl;
	std::cout << std::setw(10) << std::left << second << ": "
		<< tourScores_[1] << " scores" << std::endl;
	std::cout << std::setw(10) << std::left << third << ": "
		<< tourScores_[2] << " scores" << std::endl;

	std::cout << "++++++++++++++++++++++" << std::endl;
}

void Mode::printFinalScores(const std::vector<std::string>& strategyName) {
	for (size_t i = 0; i < strategyName.size(); ++i) {
		std::cout << "\n" << std::setw(10) << std::left << strategyName[i] << ": "
			<< finalScores[strategyName[i]] << " scores" << std::endl;
	}
}