// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <bounded/integer.hpp>
#include <bounded/optional.hpp>
#include <bounded/detail/to_integer.hpp>

#include "homogeneous_equals.hpp"
#include "string_view.hpp"
 
#include "../test_assert.hpp"

#include <unordered_map>
#include <sstream>

namespace {

auto check_throw_policy() {
	constexpr auto minimum = bounded::constant<0>;
	constexpr auto maximum = bounded::constant<10>;
	using policy = bounded::throw_policy<>;
	try {
		policy::assignment(20, minimum, maximum);
		BOUNDED_TEST(false);
	} catch (...) {
	}
	try {
		policy::assignment(-6, minimum, maximum);
		BOUNDED_TEST(false);
	} catch (...) {
	}
}


template<typename Optional>
constexpr auto check_empty_braces() {
	Optional op = {};
	BOUNDED_TEST(!op);
	op = {};
	BOUNDED_TEST(!op);
}

template<typename T>
constexpr auto check_uncompressed_optional() {
	using bounded::detail::normalize;
	using limits = bounded::basic_numeric_limits<T>;
	using type = bounded::integer<normalize<limits::min()>, normalize<limits::max()>>;
	static_assert(
		sizeof(type) < sizeof(bounded::optional<type>),
		"Compressing an optional that should not be compressed."
	);
	check_empty_braces<bounded::optional<type>>();
}
template<auto minimum, auto maximum>
constexpr auto check_compressed_optional() {
	using type = bounded::integer<minimum, maximum>;
	static_assert(
		sizeof(type) == sizeof(bounded::optional<type>),
		"compressed_optional too big."
	);
	check_empty_braces<bounded::optional<type>>();
}

template<typename integer_type>
constexpr auto check_integer_optional() {
	constexpr bounded::optional<integer_type> uninitialized_optional;
	static_assert(!uninitialized_optional, "Default constructor should leave uninitialized.");
	static_assert(value_or(uninitialized_optional, integer_type(9)) == integer_type(9), "value_or incorrect for uninitialized");
	constexpr bounded::optional<integer_type> constexpr_optional_integer(integer_type(5));
	static_assert(constexpr_optional_integer, "Value constructor should initialize optional.");
	static_assert(*constexpr_optional_integer == 5, "Value in an optional incorrect.");
	static_assert(value_or(constexpr_optional_integer, integer_type(9)) == integer_type(5), "value_or incorrect for initialized");

	bounded::optional<integer_type> optional_integer(integer_type(4));
	optional_integer = uninitialized_optional;
	BOUNDED_TEST(!optional_integer);

	optional_integer = integer_type(7);
	BOUNDED_TEST(optional_integer);

	static_assert(std::is_same_v<decltype(*optional_integer), integer_type &>, "Incorrect type of *optional.");
	*optional_integer = integer_type(1);
	BOUNDED_TEST(optional_integer);
	BOUNDED_TEST(*optional_integer == 1);
	
	optional_integer = bounded::none;
	BOUNDED_TEST(!optional_integer);

	check_empty_braces<bounded::optional<integer_type>>();
}

template<typename type>
constexpr auto check_non_trivial_optional() {
	bounded::optional<type> uninitialized_optional;
	BOUNDED_TEST(!uninitialized_optional);
	decltype(auto) uninitialized_value_or = value_or(uninitialized_optional, "spork");
	BOUNDED_TEST(uninitialized_value_or == "spork");
	static_assert(std::is_same_v<decltype(uninitialized_value_or), type>, "value_or incorrect for uninitialized");
	static_assert(std::is_same_v<decltype(value_or(std::move(uninitialized_optional), type("spoon"))), type &&>, "value_or incorrect for uninitialized");
	bounded::optional<type> optional_string("Hello");
	BOUNDED_TEST(optional_string);
	auto const default_value = type("knife");
	decltype(auto) initialized_value_or = value_or(optional_string, default_value);
	BOUNDED_TEST(initialized_value_or == "Hello");
	static_assert(std::is_same_v<decltype(initialized_value_or), type const &>, "value_or incorrect for initialized");
	BOUNDED_TEST(*optional_string == "Hello");

	optional_string = uninitialized_optional;
	BOUNDED_TEST(!optional_string);

	optional_string = type("Hi");
	BOUNDED_TEST(optional_string);
	optional_string = "Yo";
	BOUNDED_TEST(optional_string);
	BOUNDED_TEST(*optional_string == "Yo");
	BOUNDED_TEST(*optional_string != "Sup");

	static_assert(std::is_same_v<decltype(*optional_string), type &>, "Incorrect type of *optional.");
	*optional_string = type("Hiya");
	BOUNDED_TEST(optional_string);
	BOUNDED_TEST(*optional_string == "Hiya");
	
	optional_string = bounded::none;
	BOUNDED_TEST(!optional_string);

	check_empty_braces<bounded::optional<type>>();
}

auto check_optional() {
	static_assert([]{
		check_compressed_optional<1, 10>();
		check_compressed_optional<-50, 127>();
		check_uncompressed_optional<uint8_t>();
		check_uncompressed_optional<int>();
		check_uncompressed_optional<unsigned>();
		check_uncompressed_optional<bounded::detail::max_signed_t>();

		check_integer_optional<int>();
		check_integer_optional<bounded::checked_integer<1, 10>>();
		return true;
	}());

	check_non_trivial_optional<bounded_test::string_view>();
	check_non_trivial_optional<std::string>();

	constexpr auto original = bounded::make_optional(bounded::constant<0>);
	constexpr auto copy = original;
	static_assert(original == copy);
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
	streaming_test<bounded::checked_integer<0, 100>>(7, 0);
	constexpr auto large_initial = std::numeric_limits<int>::max() / 3;
	constexpr auto large_final = -49;
	streaming_test<decltype(bounded::integer(0))>(large_initial, large_final);
}

auto check_hash() {
	std::unordered_map<bounded::integer<0, 100>, bounded::integer<0, 100>> const map = {
		{ bounded::constant<1>, bounded::constant<2> },
		{ bounded::constant<2>, bounded::constant<3> },
		{ bounded::constant<3>, bounded::constant<5> },
		{ bounded::constant<4>, bounded::constant<7> },
		{ bounded::constant<5>, bounded::constant<11> },
		{ bounded::constant<1>, bounded::constant<0> }
	};
	
	BOUNDED_TEST(map.size() == 5);
	BOUNDED_TEST(map.at(bounded::constant<1>) == bounded::constant<2>);
	BOUNDED_TEST(map.at(bounded::constant<3>) == bounded::constant<5>);
}

auto check_to_integer() {
	try {
		bounded::to_integer<0, 0>("");
		BOUNDED_TEST(false);
	} catch (std::invalid_argument const &) {
	}
	try {
		bounded::to_integer<-1, 1>("-");
		BOUNDED_TEST(false);
	} catch (std::invalid_argument const &) {
	}
	try {
		bounded::to_integer<0, 0>("1");
		BOUNDED_TEST(false);
	} catch (std::invalid_argument const &) {
	}
	try {
		bounded::to_integer<0, 100>("101");
		BOUNDED_TEST(false);
	} catch (std::invalid_argument const &) {
	}
	try {
		bounded::to_integer<0, 100>("-90");
		BOUNDED_TEST(false);
	} catch (std::invalid_argument const &) {
	}
}

}	// namespace

auto main() -> int {
	check_throw_policy();
	check_optional();
	check_to_string();
	check_streaming();
	check_hash();
	check_to_integer();
}
