#include "game.h"


int main(int argc, char** argv) {
	Arguments opt;
	Parser pars;
	if (!pars.getArguments(argc, argv, opt)) {
		return 0;
	};
	try {
		Game game;
		game.Start(opt.strategyName, opt.mode, opt.steps, opt.filename);
	}
	catch (const std::exception& e) {
		std::cout << e.what() << std::endl;
	}
	return 0;
}