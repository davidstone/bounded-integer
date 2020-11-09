// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <bounded/integer.hpp>

#include <bounded/check_in_range.hpp>
#include <bounded/optional.hpp>
#include <bounded/to_integer.hpp>

#include "homogeneous_equals.hpp"
 
#include "../test_assert.hpp"

#include <sstream>

namespace {

auto check_check_in_range() {
	constexpr auto minimum = bounded::constant<0>;
	constexpr auto maximum = bounded::constant<10>;
	try {
		bounded::check_in_range(bounded::integer<-20, 20>(bounded::constant<20>), minimum, maximum);
		BOUNDED_TEST((false));
	} catch (...) {
	}
	try {
		bounded::check_in_range(bounded::integer<-6, 6>(bounded::constant<-6>), minimum, maximum);
		BOUNDED_TEST((false));
	} catch (...) {
	}
}


auto check_to_string() {
	auto const result = bounded::to_string(bounded::constant<4>);
	static_assert(std::is_same_v<decltype(result), std::string const>, "Incorrect type of to_string.");
	BOUNDED_TEST(result == "4");
}

template<typename integer>
auto streaming_test(int const initial, int const final) {
	integer value(initial);
	std::stringstream in;
	in << value;
	BOUNDED_TEST(in.str() == std::to_string(initial));
	std::stringstream out;
	out << final;
	out >> value;
	BOUNDED_TEST(value == final);
}

auto check_streaming() {
	streaming_test<bounded::integer<0, 100>>(7, 0);
	constexpr auto large_initial = bounded::max_value<int> / 3;
	constexpr auto large_final = -49;
	streaming_test<decltype(bounded::integer(0))>(large_initial, large_final);
}

auto check_to_integer() {
	try {
		bounded::to_integer<0, 0>("");
		BOUNDED_TEST((false));
	} catch (std::invalid_argument const &) {
	}
	try {
		bounded::to_integer<-1, 1>("-");
		BOUNDED_TEST((false));
	} catch (std::invalid_argument const &) {
	}
	try {
		bounded::to_integer<0, 0>("1");
		BOUNDED_TEST((false));
	} catch (std::invalid_argument const &) {
	}
	try {
		bounded::to_integer<0, 100>("101");
		BOUNDED_TEST((false));
	} catch (std::invalid_argument const &) {
	}
	try {
		bounded::to_integer<0, 100>("-90");
		BOUNDED_TEST((false));
	} catch (std::invalid_argument const &) {
	}
}

}	// namespace

auto main() -> int {
	check_check_in_range();
	check_to_string();
	check_streaming();
	check_to_integer();
}
