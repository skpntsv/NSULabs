#include <iostream>

#include "CSVParser.h"

int main(int argc, char const* argv[]) {
	try {
		std::ifstream file;
		file.open("test.csv");
		if (!file) {
			throw std::ifstream::failure("File is not open");
		}
		file.exceptions(std::ifstream::badbit);

		CSVParser<int, std::string> parser(file, 1);
		for (std::tuple<int, std::string> rs : parser) {
			std::cout << rs << std::endl;
		}

	}
	catch (const std::exception& e) {
		std::cout << e.what() << std::endl;
	}
	return 0;
}