#include <string>
#include <iostream>
#include <algorithm>
#include <climits>

#include "BigInt.h"

BigInt::BigInt() {
	_value = "0";
	_sign = true;
}

BigInt::BigInt(int num) {
	if (num < 0) {
		_sign = false;
		_value = std::to_string(num).erase(0, 1);
	}
	else {
		_value = std::to_string(num);
		_sign = true;
	}
}

BigInt::BigInt(std::string line) { // бросать исключение std::invalid_argument при ошибке
	_value = line;
	if (_value.empty()) {
		throw std::invalid_argument("invalid input string\n\t\t ERROR");
	}
	if (_value[0] == '-') {
		_value.erase(0, 1);
		if (_value.empty()) {
			throw std::invalid_argument("invalid input string\n\t\t ERROR");
		}
		_sign = false;
	}
	else {
		_sign = true;
	}
	if (_value[0] == '+') {
		_value.erase(0, 1);
	}
	if (_value.empty()) {
		throw std::invalid_argument("invalid input string\n\t\t ERROR");
	}
	while (_value[0] == '0') { // удаляем лидирующие нули
		_value.erase(0, 1);
		if (_value.empty()) {
			_value = "0";
			break;
		}
	}
	if (_value.find_first_not_of("0123456789") != std::string::npos) {
		throw std::invalid_argument("invalid input string\n\t\t ERROR");
	}
}

BigInt::BigInt(const char* line) : BigInt(std::string(line)) {} // конструктор для char[]

BigInt::BigInt(const BigInt& num) { // конструктор копирования
	*this = num;
}

BigInt::~BigInt() { //убрать всё и сделать дефолтным
}


BigInt& BigInt::operator=(const BigInt& bignum) {
	_value = bignum._value;
	_sign = bignum._sign;

	return *this;
}

BigInt BigInt::operator~() const {
	BigInt num = *this;
	std::string str;
	size_t len = num._value.size();
	str = decToBin(num);
	len = str.size();
	for (int i = 0; i < len; i++) {
		if (str[i] == '0') {
			str[i] = '1';
		}
		else {
			str[i] = '0';
		}
	}
	num = binToDec(str);

	return num;
}

BigInt& BigInt::operator++() {

	return (*this += 1);
}

const BigInt BigInt::operator++(int) {
	BigInt bignum = *this;
	++(*this);

	return bignum;
}

BigInt& BigInt::operator--() {

	return (*this -= 1);
}

const BigInt BigInt::operator--(int) {
	BigInt bignum = *this;
	--(*this);

	return bignum;
}


BigInt& BigInt::operator+=(const BigInt& bignum) {
	if (_sign == true && bignum._sign == false) {
		return ((*this) -= (-bignum));
	}
	if (_sign == false && bignum._sign == true) {
		_sign = true;
		*this = bignum - *this;
		return *this;
	}
	BigInt bignum1;
	BigInt bignum2;
	if (bignum._value.size() > _value.size()) {
		bignum1 = bignum;
		bignum2 = *this;
	}
	else {
		bignum1 = *this;
		bignum2 = bignum;
	}
	BigInt bignumresult;
	bignumresult._value.clear();
	if (bignum1._sign == bignum2._sign) {
		bignumresult._sign = bignum1._sign;
		reverse(bignum1._value.begin(), bignum1._value.end());
		reverse(bignum2._value.begin(), bignum2._value.end());
		int carry = 0; // остаток в уме
		int rank;  // цифра в разряде нового числа
		for (int i = 0; i < bignum1._value.size(); i++) {
			if (bignum2._value.size() <= i) {
				rank = bignum1._value[i] - '0';
				if (carry == 1) {
					rank++;
					if (rank != 10)
						carry = 0;
					else
						rank -= 10;
				}
				bignumresult._value.push_back(rank + '0');
			}
			else {
				rank = (bignum1._value[i] - '0') + (bignum2._value[i] - '0');
				if (rank + carry >= 10) {
					rank -= 10;
					bignumresult._value.push_back(rank + carry + '0');
					carry = 1;
				}
				else {
					bignumresult._value.push_back(rank + carry + '0');
					carry = 0;
				}
			}
		}
		if (carry == 1) {
			bignumresult._value.push_back('1');
		}
		reverse(bignumresult._value.begin(), bignumresult._value.end());
	}
	*this = bignumresult;

	return *this;
}

BigInt& BigInt::operator*=(const BigInt& bignum) {
	BigInt bignum1 = *this;
	BigInt bignum2 = bignum;
	BigInt bignumresult;
	BigInt temp;
	if (*this == bignumresult || bignum == bignumresult) {
		*this = bignumresult;
		return *this;
	}
	bignum1._sign = true;
	bignum2._sign = true;
	if (bignum1 < bignum2) {
		bignum2 = *this;
		bignum1 = bignum;
	}
	reverse(bignum1._value.begin(), bignum1._value.end());
	reverse(bignum2._value.begin(), bignum2._value.end());
	int carry = 0; // остаток в уме
	int tmp = 0;  // цифра в разряде нового числа
	for (int i = 0; i < bignum2._value.size(); i++) {
		temp._value.clear();
		for (int z = 0; z < i; z++) {
			temp._value.push_back('0');
		}
		carry = 0;
		for (int j = 0; j < bignum1._value.size(); j++) {
			tmp = (bignum1._value[j] - '0') * (bignum2._value[i] - '0');
			tmp += carry;
			if (tmp >= 10) {
				carry = tmp / 10;
				tmp = tmp % 10;
			}
			else {
				carry = 0;
			}
			temp._value.push_back(tmp + '0');
		}
		if (carry != 0) {
			temp._value.push_back(carry + '0');
		}
		reverse(temp._value.begin(), temp._value.end());
		bignumresult += temp;

	}
	//size увеличивать не нужно, он автоматически увеличивается
	//ставим знак для большого числа
	if (_sign != bignum._sign) {
		bignumresult._sign = false;
	}
	else {
		bignumresult._sign = true;
	}
	*this = bignumresult;

	return *this;
}

BigInt& BigInt::operator-=(const BigInt& bignum) {
	if (_sign == false && bignum._sign == true) {
		return (*this += -bignum);
	}
	if (_sign == true && bignum._sign == false) {
		return (*this += -bignum);
	}
	BigInt bignum1, bignum2;
	BigInt bignumresult;
	if (*this == bignum) {
		*this = bignumresult;
		return (*this);
	}
	if (*this > bignum) {
		bignumresult._sign = true;
	}
	if (*this < bignum) {
		bignumresult._sign = false;
	}
	if (bignum._value.size() == _value.size()) {
		if (*this > bignum) {
			bignum1 = *this;
			bignum2 = bignum;
		}
		else {
			bignum2 = *this;
			bignum1 = bignum;
		}
	}
	else {
		if (bignum._value.size() < _value.size()) {
			bignum1 = *this;
			bignum2 = bignum;
		}
		else {
			bignum1 = bignum;
			bignum2 = *this;
		}
	}
	bignumresult._value.clear();
	// начало вычитания
	reverse(bignum1._value.begin(), bignum1._value.end());
	reverse(bignum2._value.begin(), bignum2._value.end());
	int carry = 0; // остаток в уме
	int rank;  // цифра в разряде нового числа
	for (int i = 0; i < bignum1._value.size(); i++) {
		if (bignum2._value.size() <= i) {
			rank = bignum1._value[i] - '0';
			if (carry == 1) {
				rank--;
				carry = 0;
			}
			if (rank != 0 || ((i + 1) < bignum1._value.size())) {
				bignumresult._value.push_back(rank + '0');
			}
		}
		else 
			if (bignum1._value.substr(i) == bignum2._value.substr(i)) {
				break;
			}
		else {
			rank = (bignum1._value[i] - '0') - (bignum2._value[i] - '0');
			if (rank - carry < 0) {
				if (carry == 0) {
					rank += 10;
				}
				else {
					rank += 9;
				}
				carry = 1;
				bignumresult._value.push_back(rank + '0');
			}
			else {
				if (carry == 1)
					rank--;
				bignumresult._value.push_back(rank + '0');
				carry = 0;
			}
		}
	}
	reverse(bignumresult._value.begin(), bignumresult._value.end());
	while (bignumresult._value[0] == '0') {
		bignumresult._value.erase(0, 1);
	}
	*this = bignumresult;

	return *this;
}

BigInt& BigInt::operator/=(const BigInt& bignum) {
	if (bignum._value == "0") {
		throw std::overflow_error("ERROR: division by zero");
	}
	BigInt bignum1 = *this;
	BigInt bignum2 = bignum;
	BigInt bignumresult, tmp;
	bignum1._sign = true;
	bignum2._sign = true;
	if (bignum1 < bignum2) {
		*this = bignumresult;
		return *this;
	}
	if (bignum1 == bignum2) {
		bignumresult = 1;
		bignumresult._sign = _sign == bignum._sign;
		*this = bignumresult;
		return *this;
	}
	//очищаем большие числа
	bignumresult._value.clear();
	// простое деление в столбик
	BigInt carry; // остаток
	int j;
	bool flag = false;
	for (int i = 0; i < bignum1._value.size(); i++) {
		if (tmp._value == "0") {
			tmp._value.clear();
		}
		tmp._value.push_back(bignum1._value[i]);
		if (tmp >= bignum2) {
			for (j = 0; carry <= tmp; ++j) {
				carry += bignum2;
			}
			tmp -= (carry - bignum2);
			carry._value = "0";
			bignumresult._value.push_back((j-1) + '0');
			flag = true;
		}
		else {
			if (flag == true) {
				bignumresult._value.push_back('0');
			}
		}
	}

	//ставим знак для большого числа
	if (_sign != bignum._sign) {
		bignumresult._sign = false;
	}
	else {
		bignumresult._sign = true;
	}
	*this = bignumresult;

	return *this;
}

BigInt& BigInt::operator^=(const BigInt& bignum) {
	BigInt num = bignum;
	std::string str1 = decToBin(*this);
	std::string str2 = decToBin(num);
	std::string result;
	if (str1.size() > str2.size()) {
		addingZeroes(str2, str1.size());
	}
	if (str2.size() > str1.size()) {
		addingZeroes(str1, str2.size());
	}
	for (int i = 0; i < str1.size(); i++) {
		if (str1[i] != str2[i]) {
			result.push_back('1');
		}
		else {
			result.push_back('0');
		}
	}
	num = binToDec(result);
	num._sign = _sign == bignum._sign;
	*this = num;

	return *this;
}

BigInt& BigInt::operator%=(const BigInt& bignum) {
	BigInt result;
	BigInt q;
	BigInt s;
	q = (*this / bignum);
	s = q * bignum;
	result = *this - s;

	*this = result;

	return *this;
}

BigInt& BigInt::operator&=(const BigInt& bignum) {
	BigInt num = bignum;
	std::string str1 = decToBin(*this);
	std::string str2 = decToBin(num);
	std::string result;
	if (str1.size() > str2.size()) {
		addingZeroes(str2, str1.size());
	}
	if (str2.size() > str1.size()) {
		addingZeroes(str1, str2.size());
	}
	for (int i = 0; i < str1.size(); i++) {
		if (str1[i] == '1' && str2[i] == '1') {
			result.push_back('1');
		}
		else {
			result.push_back('0');
		}
	}
	num = binToDec(result);
	*this = num;

	return *this;
}

BigInt& BigInt::operator|=(const BigInt& bignum) {
	BigInt num = bignum;
	std::string str1 = decToBin(*this);
	std::string str2 = decToBin(num);
	std::string result;
	if (str1.size() > str2.size()) {
		addingZeroes(str2, str1.size());
	}
	if (str2.size() > str1.size()) {
		addingZeroes(str1, str2.size());
	}
	for (int i = 0; i < str1.size(); i++) {
		if (str1[i] == '1' || str2[i] == '1') {
			result.push_back('1');
		}
		else {
			result.push_back('0');
		}
	}
	num = binToDec(result);
	*this = num;

	return *this;
}


BigInt BigInt::operator+() const {
	return *this;
}

BigInt BigInt::operator-() const {
	BigInt bignum = *this;
	if (_sign == true) {
		bignum._sign = false;
	}
	else {
		bignum._sign = true;
	}

	return bignum;
}


bool BigInt::operator==(const BigInt& bignum) const {

	return _sign == bignum._sign && _value == bignum._value;
}

bool BigInt::operator!=(const BigInt& bignum) const {

	return !(*this == bignum);
}

bool BigInt::operator<(const BigInt& bignum) const {
	if (_sign == true && bignum._sign == false) {
		return false;
	}
	if (_sign == false && bignum._sign == true) {
		return true;
	}
	if (_sign == false) {
		if (_value.size() > bignum._value.size()) {
			return true;
		}
		if (_value.size() < bignum._value.size()) {
			return false;
		}
		for (int i = 0; i < _value.size(); i++) {
			if (_value[i] > bignum._value[i]) {
				return true;
			}
			if (_value[i] < bignum._value[i]) {
				return false;
			}
		}
	}
	if (_sign == true) {
		if (_value.size() > bignum._value.size()) {
			return false;
		}
		if (_value.size() < bignum._value.size()) {
			return true;
		}
		for (int i = 0; i < _value.size(); i++) {
			if (_value[i] > bignum._value[i]) {
				return false;
			}
			if (_value[i] < bignum._value[i]) {
				return true;
			}
		}
	}

	return false;
}

bool BigInt::operator>(const BigInt& bignum) const {
	if (*this == bignum) {
		return false;
	}
	if (*this < bignum) {
		return false;
	}

	return true;
}

bool BigInt::operator<=(const BigInt& bignum) const {

	return !(*this > bignum);
}

bool BigInt::operator>=(const BigInt& bignum) const {

	return !(*this < bignum);
}


BigInt::operator int() const { 
	int num = 0;
	long long power = 1;
	BigInt check_MAX = INT_MAX;
	BigInt check_MIN = INT_MIN;
	if ((*this > check_MAX) || (*this < check_MIN)) {
		throw std::length_error("Num exceeds long long int limit");
	}
	size_t i = _value.size();
	while (i-- > 0) {
		num += (_value[i] - '0') * power;
		power *= 10;
	}
	if (!_sign) {
		num *= -1;
	}
	return num;
}

BigInt::operator std::string() const {
	std::string line = _value;
	if (!_sign) {
		line.insert(0, "-");
	}
	return line;
}

size_t BigInt::Size() const {
	return (_value.size() + sizeof(bool) + sizeof(size_t));
}


std::string BigInt::decToBin(BigInt& bignum) const {
	BigInt num = bignum;
	BigInt num_zero = 0;
	num._sign = true;
	std::string line;
	short last_value = 0; // остаток от деления на 2: 1 или 0
	while (num > num_zero) {
		last_value = (num._value[num._value.size() - 1] - '0') % 2;
		if (last_value == 0) {
			line.push_back('0');
		}
		else {
			line.push_back('1');
		}
		num /= 2;
	}
	while (((line.size() - 1) % (32 - 1)) != 0) {
		line.push_back('0');
	}
	
	if (!bignum._sign) { //если число отрицательное - преобразуем его в обратный код
		line.push_back('1');
		size_t len = line.size();
		for (int i = 0; i < len - 1; i++) {
			if (line[i] == '0') {
				line[i] = '1';
			}
			else {
				line[i] = '0';
			}
		}
		int i;
		for (i = 0; i < len - 1; i++) { // прибавляем единицу к двоичному коду
			if (line[i] == '0') {
				line[i]++;
				break;
			}
			else {
				line[i] = '0';
			}
		}
		if (i == len - 2) {
			throw std::overflow_error("undefined behavior\n\t\t ERROR");
		}
	}
	else {
		line.push_back('0');
	}
	reverse(line.begin(), line.end());

	return line;
}

BigInt BigInt::binToDec(std::string &line) const {
	size_t len = line.size();
	BigInt power = 1;
	BigInt bignumresult;
	bool flag = true;
	if (line[0] == '1') {
		flag = false;
		int i;
		for (i = len - 1; i > 0; i--) { // отнимаем единичку от обратного кода
			if (line[i] == '1') {
				line[i]--;
				break;
			}
			else {
				line[i] = '1';
			}
		}
		if (i == 1) {
			throw std::overflow_error("undefined behavior\n\t\t ERROR");
		}
		for (int i = 0; i < len; i++) {
			if (line[i] == '0') {
				line[i] = '1';
			}
			else {
				line[i] = '0';
			}
		}
	}
	for (int i = len - 1; i >= 0; i--) {
		if (line[i] == '1') {
			bignumresult += power;
		}
		power *= 2;
	}
	bignumresult._sign = flag;

	return bignumresult;
}

void BigInt::addingZeroes(std::string &line, size_t amt) const {
	bool flag = true;
	if (line[0] == '1') {
		flag = false;
		line[0] = '0';
	}
	reverse(line.begin(), line.end());
	while (line.size() < amt) {
		line.push_back('0');
	}
	reverse(line.begin(), line.end());
	if (flag == false) {
		line[0] = '1';
	}
}


BigInt operator+(const BigInt &bignum1, const BigInt &bignum2) {
	BigInt result = bignum1;
	result += bignum2;

	return result;
}

BigInt operator-(const BigInt& bignum1, const BigInt& bignum2) {
	BigInt result = bignum1;
	result -= bignum2;

	return result;
}

BigInt operator*(const BigInt& bignum1, const BigInt& bignum2) {
	BigInt result = bignum1;
	result *= bignum2;

	return result;
}

BigInt operator/(const BigInt& bignum1, const BigInt& bignum2) {
	BigInt result = bignum1;
	result /= bignum2;

	return result;
}

BigInt operator^(const BigInt& bignum1, const BigInt& bignum2) {
	BigInt result = bignum1;
	result ^= bignum2;

	return result;
}

BigInt operator%(const BigInt& bignum1, const BigInt& bignum2) {
	BigInt result = bignum1;
	result %= bignum2;

	return result;
}

BigInt operator&(const BigInt& bignum1, const BigInt& bignum2) {
	BigInt result = bignum1;
	result &= bignum2;

	return result;
}

BigInt operator|(const BigInt& bignum1, const BigInt& bignum2) {
	BigInt result = bignum1;
	result |= bignum2;

	return result;
}


std::ostream& operator<<(std::ostream& out, const BigInt& bignum) {
	out << static_cast<std::string>(bignum);

	return out;
}