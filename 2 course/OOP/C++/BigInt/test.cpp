
#include "gtest/gtest.h"
#include "BigInt.h"
#include <climits>

TEST(Constructor, Int) {
	BigInt test;

	test = 12;
	EXPECT_EQ(static_cast<int>(test), 12);
	EXPECT_EQ(static_cast<std::string>(test), "12");

	test = -14352;
	EXPECT_EQ(static_cast<int>(test), -14352);
	EXPECT_EQ(static_cast<std::string>(test), "-14352");

	test = INT_MAX;
	EXPECT_EQ(static_cast<int>(test), INT_MAX);
	EXPECT_EQ(static_cast<std::string>(test), "2147483647");

	test = INT_MIN;
	EXPECT_EQ(static_cast<int>(test), INT_MIN);
	EXPECT_EQ(static_cast<std::string>(test), "-2147483648");
}
TEST(Constructor, string) {
	BigInt test;

	test = "10930";
	EXPECT_EQ(static_cast<int>(test), 10930);
	EXPECT_EQ(static_cast<std::string>(test), "10930");

	test = "-04124";
	EXPECT_EQ(static_cast<int>(test), -4124);
	EXPECT_EQ(static_cast<std::string>(test), "-4124");
}
TEST(Constructor, all_Zeroes) {
	BigInt test("000000000");
	EXPECT_EQ(static_cast<int>(test), 0);
	EXPECT_EQ(static_cast<std::string>(test), "0");
}
TEST(Constructor, plus_is_first) {
	BigInt test("+20");
	EXPECT_EQ(static_cast<int>(test), 20);
	EXPECT_EQ(static_cast<std::string>(test), "20");
}
TEST(Constructor, first_are_zeroes) {
	BigInt test("000000412");
	EXPECT_EQ(static_cast<int>(test), 412);
	EXPECT_EQ(static_cast<std::string>(test), "412");
}
TEST(Operator_output, output) {
	std::ostringstream out;
	out << BigInt("-341341413");
	EXPECT_STREQ(out.str().data(), "-341341413");
}
TEST(Operator_plus, test1) {
	BigInt result = 0;
	BigInt a = 189;
	BigInt b = -189;
	BigInt tmp;
	tmp = a + b;
	EXPECT_EQ(tmp, result);
	tmp = -a + -b;
	EXPECT_EQ(tmp, result);
	EXPECT_TRUE(tmp == result);
	EXPECT_TRUE(a > b);
}
TEST(Operator_plus, test2) {
	BigInt result = 10009998;
	BigInt a = 9999;
	BigInt b = 9999999;
	BigInt tmp;
	tmp = a + b;
	EXPECT_EQ(tmp, result);
	EXPECT_TRUE(tmp == result);
	EXPECT_TRUE(a < b);
}
TEST(Operator_minus, test1){
	BigInt result = -9999;
	BigInt a = 0;
	BigInt b = 9999;
	BigInt tmp;
	tmp = a - b;
	EXPECT_EQ(tmp, result);
	EXPECT_TRUE(tmp == result);
	EXPECT_FALSE(tmp != +result);
}
TEST(Operator_mul, test1) {
	BigInt result = -123;
	BigInt a = -123;
	BigInt b = 1;
	BigInt tmp;
	tmp = a * b;
	EXPECT_EQ(tmp, result);
	EXPECT_TRUE(tmp == result);
	EXPECT_TRUE(a != b);
}
TEST(Operator_mul, test2) {
	BigInt result = 15129;
	BigInt a = 123;
	BigInt b = -123;
	BigInt tmp;
	tmp = -a * b;
	EXPECT_EQ(tmp, result);
	EXPECT_TRUE(-a >= b);
	EXPECT_TRUE(-a <= b);
}
TEST(Operator_mul, test3) {
	BigInt result = 151782;
	BigInt a = 123;
	BigInt b = 1234;
	BigInt tmp;
	tmp = a * b;
	EXPECT_EQ(tmp, result);
	EXPECT_TRUE(a <= b);
	EXPECT_TRUE(b >= a);
}
TEST(Operator_mul, big_test) {
	std::string s = "9999999999999999999999999999999999999999999999999999999999999999999999999999999999";
	BigInt result = "-9999999999999999999999999999999999999999999999999999999999999999999999999999999998999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999990000000000000000000000000000000000000000000000000000000000000000000000000000000001";
	BigInt a = s;
	BigInt b = "-999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999";
	BigInt tmp;
	tmp = a * b;
	EXPECT_EQ(tmp, result);
}
TEST(Operator_mix, test1) {
	BigInt result = 100;
	BigInt a = 0;
	BigInt b = -10;
	BigInt tmp;
	tmp = a * b;
	tmp += b;
	tmp *= b;
	EXPECT_EQ(tmp, result);
	EXPECT_TRUE(a >= b);
	EXPECT_TRUE(b <= a);
}
TEST(Operator_plus, test3) {
	BigInt result = 0;
	BigInt a = -189;
	BigInt b = 189;
	BigInt tmp;
	a += b;
	EXPECT_EQ(tmp, result);
}
TEST(Operator_minus, test2) {
	BigInt result = -10000;
	BigInt a = -1;
	BigInt b = 9999;
	BigInt tmp;
	tmp = a - b;
	EXPECT_EQ(tmp, result);
	tmp = b - a;
	EXPECT_EQ(tmp, -result);
	tmp = b - -a;
	result = 9998;
	EXPECT_EQ(tmp, result);
}
TEST(Operator_minus, test3) {
	BigInt result = -8;
	BigInt a = 9991;
	BigInt b = 9999;
	BigInt tmp;
	tmp = a - b;
	EXPECT_EQ(tmp, result);
	tmp = b - a;
	EXPECT_EQ(tmp, -result);
	EXPECT_FALSE(-a <= -b);
	EXPECT_TRUE(-b < -a);
}
TEST(Operator_minus, test4) {
	BigInt result = -8008;
	BigInt a = 1991;
	BigInt b = 9999;
	BigInt tmp;
	tmp = a - b;
	EXPECT_EQ(tmp, result);
	tmp = b - a;
	EXPECT_EQ(tmp, -result);
	EXPECT_FALSE(a >= b);
}
TEST(Operator_minus, test5) {
	BigInt result = "135904956199";
	BigInt a = "135904991111";
	BigInt b = 34912;
	BigInt tmp;
	tmp = a - b;
	EXPECT_EQ(tmp, result);
	EXPECT_TRUE(-a < -b);
	EXPECT_FALSE(-b <= -a);
}
TEST(Operator_division, test1) {
	BigInt result = "176";
	BigInt a = "528";
	BigInt b = 3;
	BigInt tmp;
	tmp = a / b;
	EXPECT_EQ(tmp, result);
}
TEST(Operator_division, test2) {
	BigInt result = "-88";
	BigInt a = -528;
	BigInt b = 6;
	BigInt tmp;
	tmp = a / b;
	EXPECT_EQ(tmp, result);
}
TEST(Operator_division, test3) {
	BigInt result = -72;
	BigInt a = 1368;
	BigInt b = -19;
	BigInt tmp;
	tmp = a / b;
	EXPECT_EQ(tmp, result);
}
TEST(Operator_division, test4) {
	BigInt result = 805;
	BigInt a = -30590;
	BigInt b = -38;
	BigInt tmp;
	tmp = a / b;
	EXPECT_EQ(tmp, result);
}
TEST(Operator_division, test5) {
	BigInt result = 104;
	BigInt a = 2808;
	BigInt b = 27;
	BigInt tmp;
	tmp = a / b;
	EXPECT_EQ(tmp, result);
}
TEST(Operator_division, test6) {
	BigInt result = 201;
	BigInt a = 2412;
	BigInt b = 12;
	BigInt tmp;
	tmp = a / b;
	EXPECT_EQ(tmp, result);
}
TEST(Operator_division, test7) {
	BigInt result = 99;
	BigInt a = 598;
	BigInt b = 6;
	BigInt tmp;
	tmp = a / b;
	EXPECT_EQ(tmp, result);
}
TEST(Operator_division, test8) {
	BigInt result = 1;
	BigInt a = 32;
	BigInt b = 32;
	BigInt tmp;
	tmp = a / b;
	EXPECT_EQ(tmp, result);
}
TEST(Operator_division, test9) {
	BigInt result = 0;
	BigInt a = 2;
	BigInt b = 5;
	BigInt tmp;
	tmp = a / b;
	EXPECT_EQ(tmp, result);
}
TEST(Operator_division, big_test) {
	BigInt result = "15097560452844284928371075057700";
	BigInt a = "89124798124791049102412094048971289471289718947194717284781471728949124981209481209480912";
	BigInt b = "5903258238518958913412414141241247582385723895789237559237";
	BigInt tmp;
	tmp = a / b;
	EXPECT_EQ(tmp, result);
}
TEST(Operator_inc_and_dec, prefix) {
	BigInt result;
	BigInt tmp;

	tmp = 0;
	result = 1;
	++tmp;
	EXPECT_EQ(tmp, result);
	
	tmp = 0;
	result = -1;
	--tmp;
	EXPECT_EQ(tmp, result);

	result = "-89124798124791049102412094048971289471289718947194717284781471728949124981209481209480911";
	tmp = "-89124798124791049102412094048971289471289718947194717284781471728949124981209481209480912";
	++tmp;
	EXPECT_EQ(tmp, result);

	result = "-89124798124791049102412094048971289471289718947194717284781471728949124981209481209480913";
	tmp = "-89124798124791049102412094048971289471289718947194717284781471728949124981209481209480912";
	--tmp;
	EXPECT_EQ(tmp, result);
}
TEST(Operator_inc_and_dec, postfix) {
	BigInt result;
	BigInt tmp;

	tmp = 0;
	result = 0;
	EXPECT_EQ(tmp++, result);
	EXPECT_EQ(tmp, ++result);

	tmp = 0;
	result = 0;
	EXPECT_EQ(tmp--, result);
	EXPECT_EQ(tmp, --result);

	result = "-89124798124791049102412094048971289471289718947194717284781471728949124981209481209480911";
	tmp = "-89124798124791049102412094048971289471289718947194717284781471728949124981209481209480912";
	tmp++;
	EXPECT_EQ(tmp, result);

	result = "-89124798124791049102412094048971289471289718947194717284781471728949124981209481209480913";
	tmp = "-89124798124791049102412094048971289471289718947194717284781471728949124981209481209480912";
	tmp--;
	EXPECT_EQ(tmp, result);
}
TEST(Test_bit, Bit_OR) {
	BigInt result;
	BigInt a;
	BigInt b;
	BigInt tmp;

	result = 2 | 124124;
	a = 2;
	b = 124124;
	tmp = a | b;
	EXPECT_EQ(tmp, result);

	result = -34122 | -124;
	a = -34122;
	b = -124;
	tmp = a | b;
	EXPECT_EQ(tmp, result);

	result = "481728457128947124591259135";
	tmp = BigInt("481728457128947124591259125") | BigInt("42");
	EXPECT_EQ(tmp, result);	
	
	result = "-481728457128947122752716805";
	tmp = BigInt("-13") | BigInt("-481728457128947124591259125"); //-481728457128947122752716805
	EXPECT_EQ(tmp, result);
}
TEST(Test_bit, Bit_AND) {
	long long result;
	BigInt a;
	BigInt b;
	BigInt tmp;

	result = 522 & 99124;
	a = 522;
	b = 99124;
	tmp = a & b;
	EXPECT_EQ(tmp, BigInt(result));

	result = 3442442122 & -124;
	a = 3442442122;
	b = -124;
	tmp = a & b;
	EXPECT_EQ(tmp, BigInt(result));

	result = 432;
	tmp = BigInt("481728457128947124591259125") & BigInt("432"); //432
	EXPECT_EQ(tmp, BigInt(result));
	
	result = 9;
	tmp = BigInt("13") & BigInt("-481728457128947124591259125"); //9
	EXPECT_EQ(tmp, BigInt(result));
}
TEST(Test_bit, Bit_XOR) {
	long long result;
	BigInt a;
	BigInt b;
	BigInt tmp;
	BigInt res;

	result = -984 ^ 99124;
	a = -984;
	b = 99124;
	tmp = a ^ b;
	EXPECT_EQ(tmp, BigInt(result));

	result = 1234567890 ^ -4;
	a = 1234567890;
	b = -4;
	tmp = a ^ b;
	EXPECT_EQ(tmp, BigInt(result));

	res = "481728457128947124591258693";
	tmp = BigInt("481728457128947124591259125") ^ BigInt("432");
	EXPECT_EQ(tmp, res);

	res = "-481728457128947124591259130";
	tmp = BigInt("13") ^ BigInt("-481728457128947124591259125"); //-481728457128947124591259130
	EXPECT_EQ(tmp, res);
}
TEST(Test_bit, Bit_NOT) {
	long long result;
	BigInt tmp;

	result = ~1444444;
	tmp = "1444444";
	tmp = ~tmp;
	EXPECT_EQ(tmp, BigInt(result));

	result = ~-4739;
	tmp = -4739;
	tmp = ~tmp;
	EXPECT_EQ(tmp, BigInt(result));
}
TEST(Operator_remainder_of_div, test1) {
	BigInt result;
	BigInt tmp;

	result = 341;
	tmp = BigInt(438974) % BigInt(489);
	EXPECT_EQ(tmp, result);

	result = "196887502";
	tmp = BigInt("481297489172491294") % BigInt("478124194");
	EXPECT_EQ(tmp, result);
}
TEST(Operator_remainder_of_div, test2) {
	int result;
	BigInt tmp;

	result = -4 % 32;
	tmp = BigInt(-4) % BigInt(32);
	EXPECT_EQ(static_cast<int>(tmp), result);

	result = 78 % -3;
	tmp = BigInt(78) % BigInt("-3");
	EXPECT_EQ(static_cast<int>(tmp), result);

	result = -1 % -4;
	tmp = BigInt(-1) % BigInt(-4);
	EXPECT_EQ(static_cast<int>(tmp), result);
}
TEST(Test_Constructor, string) {
	std::string result;
	std::string str;
	BigInt tmp;

	result = "-32";
	tmp = -32;
	EXPECT_STREQ(static_cast<std::string>(tmp).data(), result.data());

	result = "2";
	tmp = 2;
	EXPECT_STREQ(static_cast<std::string>(tmp).data(), result.data());
}
TEST(Test_Constructor, size) {
	size_t result;
	BigInt tmp;

	result = 11;
	tmp = -32;
	EXPECT_EQ(tmp.Size(), result);

}
TEST(Errors, string_is_empty) {
	try {
		BigInt bignum("");
	}
	catch (const std::invalid_argument& e) {
		EXPECT_STREQ(e.what(), "invalid input string\n\t\t ERROR");
	}
}
TEST(Errors, invalid_input1) {
	try {
		BigInt bignum("--12451");
	}
	catch (const std::invalid_argument& e) {
		EXPECT_STREQ(e.what(), "invalid input string\n\t\t ERROR");
	}
}
TEST(Errors, invalid_input2) {
	try {
		BigInt bignum("4314e41");
	}
	catch (const std::invalid_argument& e) {
		EXPECT_STREQ(e.what(), "invalid input string\n\t\t ERROR");
	}
}
TEST(Errors, division_by_zero) {
	try {
		BigInt a = 4214;
		BigInt b = 0;
		a /= b;
	}
	catch (const std::overflow_error& e) {
		EXPECT_STREQ(e.what(), "ERROR: division by zero");
	}
}
TEST(Errors, lenght_error_to_convert_to_int1) {
	try {
		BigInt a = INT_MAX;
		a++;
		int num = int(a);
	}
	catch (const std::length_error& e) {
		EXPECT_STREQ(e.what(), "Num exceeds long long int limit");
	}
}
TEST(Errors, lenght_error_to_convert_to_int2) {
	try {
		BigInt a = INT_MIN;
		a--;
		int num = int(a);
	}
	catch (const std::length_error& e) {
		EXPECT_STREQ(e.what(), "Num exceeds long long int limit");
	}
}
TEST(Errors, overflowbits1) {
	try {
		BigInt tmp;
		tmp = 2147483647;
		~tmp;
	}
	catch (const std::overflow_error& e) {
		EXPECT_STREQ(e.what(), "undefined behavior\n\t\t ERROR");
	}
}
TEST(Errors, overflowbits2) {
	try {
		BigInt tmp;
		tmp = -2147483648;
		~tmp;
	}
	catch (const std::overflow_error& e) {
		EXPECT_STREQ(e.what(), "undefined behavior\n\t\t ERROR");
	}
}
TEST(Errors, solo_plus) {
	try {
		BigInt tmp;
		tmp = "+";
	}
	catch (const std::invalid_argument& e) {
		EXPECT_STREQ(e.what(), "invalid input string\n\t\t ERROR");
	}
}
TEST(Errors, solo_minus) {
	try {
		BigInt tmp;
		tmp = "-";
	}
	catch (const std::invalid_argument& e) {
		EXPECT_STREQ(e.what(), "invalid input string\n\t\t ERROR");
	}
}

int main(int argc, char** argv)
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}