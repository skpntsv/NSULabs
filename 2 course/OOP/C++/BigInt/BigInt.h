#pragma once

class BigInt {
private:
	std::string _value;
	bool _sign;

	std::string decToBin(BigInt&) const;
	BigInt binToDec(std::string&) const;
	void addingZeroes(std::string&, size_t amt) const;

public:
	BigInt(); //структура большого инта
	BigInt(int); // конструктор 
	BigInt(std::string); // бросать исключение std::invalid_argument при ошибке
	BigInt(const char*); // добавил конструктор для массива char
	BigInt(const BigInt&); // конструктор копирования
	~BigInt(); // деконструктор

	BigInt& operator=(const BigInt&);  //возможно присваивание самому себе!

	BigInt operator~() const;

	BigInt& operator++(); // префискный инкремент
	const BigInt operator++(int); // постфиксный инкремент
	BigInt& operator--(); // префискный инкремент
	const BigInt operator--(int); // постфиксный инкремент

	BigInt& operator+=(const BigInt&);
	BigInt& operator*=(const BigInt&);
	BigInt& operator-=(const BigInt&);
	BigInt& operator/=(const BigInt&);

	BigInt& operator^=(const BigInt&);
	BigInt& operator%=(const BigInt&);
	BigInt& operator&=(const BigInt&);
	BigInt& operator|=(const BigInt&);

	BigInt operator+() const;  // unary +
	BigInt operator-() const;  // unary -

	bool operator==(const BigInt&) const;
	bool operator!=(const BigInt&) const;
	bool operator<(const BigInt&) const;
	bool operator>(const BigInt&) const;
	bool operator<=(const BigInt&) const;
	bool operator>=(const BigInt&) const;

	operator int() const;
	operator std::string() const;

	size_t Size() const;  // size in bytes
};

BigInt operator+(const BigInt&, const BigInt&);
BigInt operator-(const BigInt&, const BigInt&);
BigInt operator*(const BigInt&, const BigInt&);
BigInt operator/(const BigInt&, const BigInt&);
BigInt operator^(const BigInt&, const BigInt&);
BigInt operator%(const BigInt&, const BigInt&);
BigInt operator&(const BigInt&, const BigInt&);
BigInt operator|(const BigInt&, const BigInt&);


std::ostream& operator<<(std::ostream& out, const BigInt& bignum);
