#pragma once

#include <iostream>

#include <tuple>


template <size_t> struct int_{}; // шаблонный int, который будет передевать в print_tuple

template <class Tuple>
std::ostream& print_tuple(std::ostream& out, const Tuple& t, int_<1>) {
	out << std::get<std::tuple_size<Tuple>::value - 1>(t);

	return out;
}

template <class Tuple, size_t Position>
std::ostream& print_tuple(std::ostream& out, const Tuple& t, int_<Position>) {
	out << std::get<std::tuple_size<Tuple>::value - Position>(t) << ", ";

	return print_tuple(out, t, int_<Position - 1>());
}


template <class... Args>
std::ostream& operator<<(std::ostream& out, const std::tuple<Args...>& t) {
	out << "(";
	print_tuple(out, t, int_<sizeof...(Args)>());

	return out << ")\n";
}
