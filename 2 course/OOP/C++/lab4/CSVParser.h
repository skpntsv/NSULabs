#pragma once

#include <sstream>
#include <fstream>

#include <string>
#include "print_tuple.h"

#define columnDelimiter ','

template <class... Args>
class CSVParser {
private:
	std::ifstream* _input;
	size_t _lineCounter = 1;

	template<class Tuple>
	std::istringstream& readEl(std::istringstream& input, Tuple& t) {
		std::string str;
		getline(input, str, columnDelimiter);
		std::istringstream stream(str);
		stream >> t;
		if (stream.fail()) {
			throw std::invalid_argument("Wrong data in " + std::to_string(_lineCounter) + " line");
		}

		return input;
	}

	template<class Tuple, size_t Position>
	std::istringstream& readTuple(std::istringstream& input, Tuple& t, int_<Position>) {
		readEl(input, std::get<std::tuple_size<Tuple>::value - Position>(t));

		return readTuple(input, t, int_<Position - 1>());
	}

	template<class Tuple>
	std::istringstream& readTuple(std::istringstream& input, Tuple& t, int_<1>) {
		readEl(input, std::get<std::tuple_size<Tuple>::value - 1>(t));

		return input;
	}

protected:
	template <class... Arg>
	std::ifstream& getLine(std::ifstream& input, std::tuple<Arg...>& t, size_t& lineCounter) {
		_lineCounter = lineCounter;
		std::string str;
		getline(input, str);
		std::istringstream stream(str);
		readTuple(stream, t, int_<sizeof ...(Arg)>());
		lineCounter++;

		return input;
	}

public:
	CSVParser() = default;
	CSVParser(std::ifstream& input, size_t skip) {
		std::string line;
		for (size_t i = 0; i < skip; ++i) {
			getline(input, line);
		}
		_input = &input;
		_lineCounter += skip;
	}
	~CSVParser() = default;

	class InputIterator : CSVParser<InputIterator> {
	private:
		std::ifstream* _input;
		std::tuple<Args...> _Tuple;
		size_t _pos = 0;
		size_t _lineCount = 1;

	public:
		InputIterator(std::ifstream* input) :
			_input(input) {
			if (input == nullptr) {
				_pos = -1;
			}
		}
		InputIterator(std::ifstream* input, size_t& lineCounter) :
			_input(input),
			_lineCount(lineCounter) {
			if (input == nullptr) {
				_pos = -1;
			}
			else {
				this->getLine(*input, _Tuple, _lineCount);
			}
		}

		~InputIterator() = default;

		bool operator==(const InputIterator& other) const {

			return (_input == other._input) && (_pos == other._pos);
		}
		bool operator!=(const InputIterator& other) const {

			return !(*this == other);
		}

		InputIterator& operator++() {
			if (_input == nullptr) {
				_pos = -1;

				return *this;
			}
			this->getLine(*_input, _Tuple, _lineCount);
			if (_input->eof()) {
				++_pos;
				_input = nullptr;

				return *this;
			}
			++_pos;

			return *this;
		}

		std::tuple<Args...> operator*() {

			return _Tuple;
		}
		std::tuple<Args...> operator->() {

			return &_Tuple;
		}

	};

	InputIterator begin() {

		return InputIterator(_input, _lineCounter);
	}
	InputIterator end() {

		return nullptr;
	}
};