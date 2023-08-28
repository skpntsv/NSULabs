#include <iostream>

#include <vector>
#include <string>

#include <algorithm>
#include <iterator>
#include <map>
#include <string_view>

enum class GameMode {
	detailed,
	tournament,
	fast
};

struct Arguments {
public:
	std::vector<std::string> strategyName;
	GameMode mode = GameMode::detailed;
	std::string filename;
	int steps = 8;
};

class Parser {
public:
	bool getArguments(int agrc, char* argv[], Arguments& opt);

private:
	std::vector<std::string> allStrategies_ = {
		"randomator", "cooperator", "defector", "copycat", "grudger", "softcopycat"
	};
	std::vector<std::string> prefixes_ = {
		"--mode=", "--steps=", "--matrix=", "--help", "-h", "help"
	};
	std::map<std::string, GameMode> modeMap_ = { 
		{"tournament", GameMode::tournament},
		{"detailed", GameMode::detailed},
		{"fast", GameMode::fast} 
	};
	std::string mode_ = "detailed";

	void printHelpOfStrg();
	void printHelpOfKeys();
};