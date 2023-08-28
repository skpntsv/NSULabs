#include "parser.h"

bool Parser::getArguments(int argc, char* argv[], Arguments& opt) {
	if (argc > 1) {
		std::string arg = argv[1];
		size_t i = 1;
		size_t prefix_id = 0;
		if (arg == "--help") {
			printHelpOfStrg();
			printHelpOfKeys();
			return false;
		}
		while ((i < argc) && (argv[i][0] != '-')) {
			arg = argv[i];
			auto found = std::find(std::begin(allStrategies_), std::end(allStrategies_), arg);
			std::cout << "The " << i << " player: " << arg << std::endl;
			if (found != std::end(allStrategies_)) {
				opt.strategyName.push_back(arg);
			}
			else {
				printHelpOfStrg();
				return false;
			}
			++i;
		}
		if (i <= 3) {
			std::cout << "You have to write minimum free names of players" << std::endl;
			printHelpOfStrg();
			return false;
		}
		for (i; i < argc; ++i) {
			prefix_id = 0;
			arg = argv[i];
			for (prefix_id; prefix_id < prefixes_.size(); ++prefix_id) {
				if (arg.rfind(prefixes_.at(prefix_id), 0) == 0) {
					break;
				}
			}
			switch (prefix_id) {
			case 0:
				mode_ = arg.substr(prefixes_.at(prefix_id).size());
				std::cout << mode_ << "Mode turn on\n" << std::endl;
				break;
			case 1:
				opt.steps = std::stoi(arg.substr(prefixes_.at(prefix_id).size()));
				break;
			case 2:
				opt.filename = arg.substr(prefixes_.at(prefix_id).size());
				std::cout << "Matrix file have loaded\n" << std::endl;
				break;
			default:
				printHelpOfStrg();
				printHelpOfKeys();
				return false;
				break;
			}
		}
	}
	else {
		printHelpOfStrg();
		printHelpOfKeys();
		return false;
	}
	auto it = modeMap_.find(mode_);
	if (it != modeMap_.end()) {
		opt.mode = it->second;
		}
	else {
		printHelpOfKeys();
		return false;
	}
	return true;
}

void Parser::printHelpOfStrg() {
	std::cout << "The first arguments have to be a name of players( min 3):\n\n" <<
		"randomator, cooperator, defector, copycat, grudger, softcopycat\n" << std::endl;
}

void Parser::printHelpOfKeys() {
	std::cout << "The second arguments have to be a keys:\n\n" <<
		"--mode=[detailed|fast|tournament] (default mode is detailed)\n" <<
		"--steps=<N> (default 8)\n" <<
		"--matrix=<filename> - Path to the file with game matrix\n" << std::endl;
}