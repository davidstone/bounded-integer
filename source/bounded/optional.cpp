// Copyright David Stone 2020.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <bounded/optional.hpp>

#include <bounded/detail/literal.hpp>
#include <bounded/integer.hpp>

#include "../test_assert.hpp"
#include "../test_int.hpp"

#include <type_traits>
 
namespace {

using namespace bounded::literal;

static_assert(
	std::same_as<std::common_type_t<bounded::none_t, int>, bounded::optional<int>>,
	"common_type with none_t first wrong."
);
static_assert(
	std::same_as<std::common_type_t<int, bounded::none_t>, bounded::optional<int>>,
	"common_type with none_t last wrong."
);
static_assert(
	std::same_as<
		std::common_type_t<bounded::constant_t<1>, bounded::none_t, bounded::constant_t<5>, bounded::none_t>,
		bounded::optional<bounded::integer<1, 5>>
	>,
	"common_type with bounded::integer and none_t wrong."
);

template<typename T>
constexpr auto test_optional_traits() {
	using bounded::optional;
	static_assert(std::is_convertible_v<T, optional<T>>);
	static_assert(std::is_trivially_destructible_v<T> == std::is_trivially_destructible_v<optional<T>>);
	static_assert(std::is_trivially_copy_constructible_v<T> == std::is_trivially_copy_constructible_v<T>);
	static_assert(std::is_trivially_copy_assignable_v<T> == std::is_trivially_copy_assignable_v<T>);
	return true;
}

struct S {
	int member;
};

static_assert(test_optional_traits<int>());
static_assert(test_optional_traits<bounded::integer<0, 10>>());
static_assert(test_optional_traits<decltype(bounded::integer(0))>());
constexpr auto test_insert() {
	auto op = bounded::optional<S>();
	bounded::insert(op, S{3});
	return op and op->member == 3;
}
static_assert(test_insert());

static_assert(*bounded::optional(5) == 5);

using integer = bounded::integer<0, 10>;
using optional = bounded::optional<integer>;

constexpr auto uninitialized = optional();

static_assert(uninitialized == bounded::none, "Fail to compare uninitialized optional to none_t.");
static_assert(uninitialized == optional(bounded::none), "Fail to compare uninitialized optional to uninitialized optional.");
static_assert(uninitialized != optional(0_bi), "Fail to compare uninitialized optional to a value.");
static_assert(bounded::none == uninitialized, "Fail to compare uninitialized optional to none_t.");
static_assert(optional(bounded::none) == uninitialized, "Fail to compare uninitialized optional to uninitialized optional.");
static_assert(optional(0_bi) != uninitialized, "Fail to compare uninitialized optional to a value.");

constexpr auto initialized = optional(5_bi);

static_assert(initialized != bounded::none, "Fail to compare initialized optional to none_t.");
static_assert(initialized != optional(bounded::none), "Fail to compare initialized optional to uninitialized optional.");
static_assert(initialized == optional(5_bi), "Fail to compare initialized optional to a value.");
static_assert(initialized != optional(4_bi), "Fail to compare initialized optional to a value.");

static_assert(bounded::none != initialized, "Fail to compare initialized optional to none_t.");
static_assert(optional(5_bi) == initialized, "Fail to compare initialized optional to a value.");
static_assert(optional(1_bi) != initialized, "Fail to compare initialized optional to a value.");

template<typename Optional>
constexpr auto check_empty_braces() {
	Optional op = {};
	BOUNDED_TEST(!op);
	op = {};
	BOUNDED_TEST(!op);
	return true;
}

template<typename T>
constexpr auto check_uncompressed_optional() {
	using type = bounded::integer<bounded::builtin_min_value<T>, bounded::builtin_max_value<T>>;
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
	constexpr auto uninitialized_optional = bounded::optional<integer_type>();
	static_assert(!uninitialized_optional, "Default constructor should leave uninitialized.");
	constexpr auto constexpr_optional_integer = bounded::optional<integer_type>(integer_type(5_bi));
	static_assert(constexpr_optional_integer, "Value constructor should initialize optional.");
	static_assert(*constexpr_optional_integer == 5, "Value in an optional incorrect.");

	auto optional_integer = bounded::optional<integer_type>(integer_type(4_bi));
	optional_integer = uninitialized_optional;
	BOUNDED_TEST(!optional_integer);

	optional_integer = integer_type(7_bi);
	BOUNDED_TEST(optional_integer);

	static_assert(std::same_as<decltype(*optional_integer), integer_type &>, "Incorrect type of *optional.");
	*optional_integer = integer_type(1_bi);
	BOUNDED_TEST(optional_integer);
	BOUNDED_TEST(*optional_integer == 1_bi);
	
	optional_integer = bounded::none;
	BOUNDED_TEST(!optional_integer);

	check_empty_braces<bounded::optional<integer_type>>();
}

template<typename type>
constexpr auto check_non_trivial_optional() {
	auto uninitialized_optional = bounded::optional<type>();
	BOUNDED_TEST(!uninitialized_optional);
	auto optional_int = bounded::optional<type>(1);
	BOUNDED_TEST(optional_int);
	BOUNDED_TEST(*optional_int == 1);

	optional_int = uninitialized_optional;
	BOUNDED_TEST(!optional_int);

	optional_int = type(2);
	BOUNDED_TEST(optional_int);
	optional_int = 3;
	BOUNDED_TEST(optional_int);
	BOUNDED_TEST(*optional_int == 3);
	BOUNDED_TEST(*optional_int != 4);

	static_assert(std::same_as<decltype(*optional_int), type &>, "Incorrect type of *optional.");
	*optional_int = type(5);
	BOUNDED_TEST(optional_int);
	BOUNDED_TEST(*optional_int == 5);
	
	optional_int = bounded::none;
	BOUNDED_TEST(!optional_int);

	check_empty_braces<bounded::optional<type>>();
}

constexpr auto check_optional() {
	check_compressed_optional<1, 10>();
	check_compressed_optional<-50, 127>();
	check_uncompressed_optional<uint8_t>();
	check_uncompressed_optional<int>();
	check_uncompressed_optional<unsigned>();
	check_uncompressed_optional<bounded::detail::max_signed_t>();

	check_integer_optional<int>();
	check_integer_optional<bounded::integer<1, 10>>();
	check_non_trivial_optional<bounded_test::integer>();

	constexpr auto original = bounded::make_optional(bounded::constant<0>);
	constexpr auto copy = original;
	static_assert(original == copy);

	return true;
}

static_assert(check_optional());

static_assert(!bounded::optional<int const &>());
static_assert(!bounded::optional<int &>());

static_assert(bounded::constructible_from<bounded::optional<int const &>, int const &>);
static_assert(bounded::constructible_from<bounded::optional<int const &>, int &&>);
static_assert(bounded::constructible_from<bounded::optional<int const &>, int &>);
static_assert(!bounded::constructible_from<bounded::optional<int &>, int const &>);
static_assert(!bounded::constructible_from<bounded::optional<int &>, int &&>);
static_assert(bounded::constructible_from<bounded::optional<int &>, int &>);
static_assert(!std::is_assignable_v<bounded::optional<int &> &, int &>);

static_assert([] {
	auto const value = 5;
	auto optional_ref = bounded::optional<int const &>(value);
	return std::addressof(*optional_ref) == std::addressof(value);
}());

static_assert([] {
	auto value = 5;
	auto optional_ref = bounded::optional<int const &>(value);
	return std::addressof(*optional_ref) == std::addressof(value);
}());

static_assert([] {
	auto value = 5;
	auto optional_ref = bounded::optional<int &>(value);
	return std::addressof(*optional_ref) == std::addressof(value);
}());

static_assert([] {
	auto value1 = 5;
	auto value2 = 5;
	auto optional_ref = bounded::optional<int &>(value1);
	optional_ref.emplace(bounded::value_to_function(value2));
	return std::addressof(*optional_ref) == std::addressof(value2);
}());

} // namespace
