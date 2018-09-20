// Copyright David Stone 2017.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#undef NDEBUG

#include <bounded/integer.hpp>
#include <bounded/optional.hpp>

#include "homogeneous_equals.hpp"
#include "string_view.hpp"
 
#include <cassert>
#include <unordered_map>
#include <sstream>

namespace {

auto check_throw_policy() {
	using bounded::checked_integer;
	static_assert(
		!noexcept(std::declval<checked_integer<0, 0> &>() = std::declval<checked_integer<0, 1> &>()),
		"Shouldn't be noexcept."
	);
	constexpr auto minimum = bounded::constant<0>;
	constexpr auto maximum = bounded::constant<10>;
	auto const policy = bounded::throw_policy{};
	try {
		policy.assignment(20, minimum, maximum);
		assert(false);
	} catch (...) {
	}
	try {
		policy.assignment(-6, minimum, maximum);
		assert(false);
	} catch (...) {
	}
}

namespace check_modulo_policy {
	constexpr bounded::wrapping_integer<0, 9> x(bounded::constant<15>);
	constexpr bounded::wrapping_integer<0, 1> y(x);
	static_assert(
		x == bounded::constant<5>,
		"Incorrect value stored with modulo policy."
	);
	static_assert(
		y == bounded::constant<1>,
		"Incorrect value stored with modulo policy."
	);
}

namespace check_clamp_policy {
	static constexpr auto minimum = bounded::constant<27>;
	static constexpr auto maximum = bounded::constant<567>;
	constexpr bounded::clamp_policy policy;
	static_assert(
		policy.assignment(bounded::constant<20>, minimum, maximum) == minimum,
		"Failure to properly clamp lesser positive values."
	);
	static_assert(
		policy.assignment(-bounded::constant<25>, minimum, maximum) == minimum,
		"Failure to properly clamp negative values to a positive value."
	);
	static_assert(
		policy.assignment(bounded::constant<1000>, minimum, maximum) == maximum,
		"Failure to properly clamp greater positive values."
	);
	static_assert(
		policy.assignment(bounded::constant<2000>, minimum, maximum) == maximum,
		"Fail to clamp above range with a strictly greater type."
	);

	using type = bounded::integer<-100, 100, bounded::clamp_policy>;
	constexpr auto initial = std::numeric_limits<type>::max().value() + 1;
	constexpr type value(initial);
	static_assert(
		value == std::numeric_limits<type>::max(),
		"Fail to clamp value when the source type is larger than the destination type."
	);


	constexpr auto integer = bounded::integer<minimum.value(), maximum.value(), bounded::clamp_policy>(bounded::constant<1000>);
	static_assert(
		integer == maximum,
		"Fail to clamp when using a bounded."
	);
}

auto check_policies() {
	check_throw_policy();
}


template<typename Optional>
constexpr auto check_empty_braces() {
	Optional op = {};
	assert(!op);
	op = {};
	assert(!op);
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
	check_empty_braces<type>();
}
template<auto minimum, auto maximum>
constexpr auto check_compressed_optional() {
	using type = bounded::integer<minimum, maximum>;
	using compressed_type = bounded::optional<type>;
	static_assert(
		sizeof(type) == sizeof(compressed_type),
		"compressed_optional too big."
	);
	static_assert(
		sizeof(compressed_type) == sizeof(bounded::optional<type>),
		"Incorrect optional selection."
	);
	check_empty_braces<compressed_type>();
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
	assert(!optional_integer);

	optional_integer = integer_type(7);
	assert(optional_integer);

	static_assert(std::is_same_v<decltype(*optional_integer), integer_type &>, "Incorrect type of *optional.");
	*optional_integer = integer_type(1);
	assert(optional_integer);
	assert(*optional_integer == 1);
	
	optional_integer = bounded::none;
	assert(!optional_integer);

	check_empty_braces<bounded::optional<integer_type>>();
}

template<typename type>
constexpr auto check_non_trivial_optional() {
	bounded::optional<type> uninitialized_optional;
	assert(!uninitialized_optional);
	decltype(auto) uninitialized_value_or = value_or(uninitialized_optional, "spork");
	assert(uninitialized_value_or == "spork");
	static_assert(std::is_same_v<decltype(uninitialized_value_or), type>, "value_or incorrect for uninitialized");
	static_assert(std::is_same_v<decltype(value_or(std::move(uninitialized_optional), type("spoon"))), type &&>, "value_or incorrect for uninitialized");
	bounded::optional<type> optional_string("Hello");
	assert(optional_string);
	auto const default_value = type("knife");
	decltype(auto) initialized_value_or = value_or(optional_string, default_value);
	assert(initialized_value_or == "Hello");
	static_assert(std::is_same_v<decltype(initialized_value_or), type const &>, "value_or incorrect for initialized");
	assert(*optional_string == "Hello");

	optional_string = uninitialized_optional;
	assert(!optional_string);

	optional_string = type("Hi");
	assert(optional_string);
	optional_string = "Yo";
	assert(optional_string);
	assert(optional_string == "Yo");
	assert(optional_string != "Sup");

	static_assert(std::is_same_v<decltype(*optional_string), type &>, "Incorrect type of *optional.");
	*optional_string = type("Hiya");
	assert(optional_string);
	assert(*optional_string == "Hiya");
	
	optional_string = bounded::none;
	assert(!optional_string);

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
	assert(result == "4");
}

template<typename integer>
auto streaming_test(int const initial, int const final) {
	integer value(initial);
	std::stringstream in;
	in << value;
	assert(in.str() == std::to_string(initial));
	std::stringstream out;
	out << final;
	out >> value;
	assert(value == final);
}

auto check_streaming() {
	streaming_test<bounded::checked_integer<0, 100>>(7, 0);
	constexpr auto large_initial = std::numeric_limits<int>::max() / 3;
	constexpr auto large_final = -49;
	streaming_test<decltype(bounded::integer(0))>(large_initial, large_final);
}

enum class bounded_enum{};

}	// namespace
namespace bounded {

template<>
struct basic_numeric_limits<bounded_enum> {
	static constexpr auto min() noexcept -> bounded::detail::max_signed_t {
		return 0;
	}
	static constexpr auto max() noexcept -> bounded::detail::max_signed_t {
		return 0;
	}
	static constexpr bool is_specialized = true;
	static constexpr bool is_integer = false;
	static constexpr bool is_bounded = true;
};
}

namespace {

namespace check_enum_construction {
	enum unscoped_enum : int {};
	static_assert(std::is_constructible<bounded::integer<0, 10>, unscoped_enum>{});
	static_assert(!std::is_convertible<unscoped_enum, bounded::integer<0, 10>>{});
	static_assert(homogeneous_equals(
		bounded::integer(unscoped_enum{}),
		bounded::integer(static_cast<std::underlying_type_t<unscoped_enum>>(0))
	));
	
	enum class scoped_enum {};
	static_assert(std::is_constructible<bounded::integer<0, 10>, scoped_enum>{});
	static_assert(!std::is_convertible<scoped_enum, bounded::integer<0, 10>>{});
	// constexpr auto b = bounded::integer(scoped_enum{});
	
	static_assert(std::is_constructible<bounded::integer<0, 10>, bounded_enum>{});
	// TODO: Should this be convertible?
	static_assert(std::is_convertible<bounded_enum, bounded::integer<0, 10>>{});
	static_assert(bounded::integer(bounded_enum{}) == bounded::constant<0>);
}

// check poisoning
static_assert(std::is_convertible<int, bounded::integer<0, 10>>{});
static_assert(std::is_convertible<decltype(std::declval<bounded::integer<0, 10>>() + 100), bounded::integer<0, 100>>{});
static_assert(std::is_convertible<decltype(bounded::integer(5)), bounded::integer<5, 5>>{});

auto check_volatile() {
	bounded::integer<0, 6> volatile x = bounded::constant<3>;
	assert(x.value() == 3);
	x = bounded::constant<2>;
	assert(x.value() == 2);
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
	
	assert(map.size() == 5);
	assert(map.at(bounded::constant<1>) == bounded::constant<2>);
	assert(map.at(bounded::constant<3>) == bounded::constant<5>);
}

}	// namespace

auto main() -> int {
	check_policies();
	check_optional();
	check_to_string();
	check_streaming();
	check_volatile();
	check_hash();
}
