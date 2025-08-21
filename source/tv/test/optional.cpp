// Copyright David Stone 2024.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

module;

#include <bounded/assert.hpp>
#include <bounded/conditional.hpp>

export module tv.test.optional;

import tv.insert;
import tv.none;
import tv.optional;
import tv.variant;

import bounded;
import bounded.test_int;
import numeric_traits;
import std_module;

using namespace bounded::literal;

static_assert(std::same_as<
	std::common_type_t<tv::none_t, int>,
	tv::optional<int>
>);
static_assert(std::same_as<
	std::common_type_t<int, tv::none_t>,
	tv::optional<int>
>);
static_assert(std::same_as<
	std::common_type_t<
		bounded::constant_t<1>,
		tv::none_t,
		bounded::constant_t<5>,
		tv::none_t
	>,
	tv::optional<bounded::integer<1, 5>>
>);

template<typename T>
constexpr auto test_optional_traits() {
	static_assert(bounded::convertible_to<T, tv::optional<T>>);
	static_assert(bounded::trivially_destructible<T> == bounded::trivially_destructible<tv::optional<T>>);
	static_assert(bounded::trivially_copy_constructible<T> == bounded::trivially_copy_constructible<T>);
	static_assert(bounded::trivially_copy_assignable<T> == bounded::trivially_copy_assignable<T>);
	return true;
}

struct S {
	int member;
};

static_assert(test_optional_traits<int>());
static_assert(test_optional_traits<bounded::integer<0, 10>>());
static_assert(test_optional_traits<decltype(bounded::integer(0))>());
constexpr auto test_insert() {
	auto op = tv::optional<S>();
	tv::insert(op, S{3});
	return op and op->member == 3;
}
static_assert(test_insert());

static_assert(*tv::optional(5) == 5);

using integer = bounded::integer<0, 10>;
using optional = tv::optional<integer>;

constexpr auto uninitialized = optional();

static_assert(uninitialized == tv::none);
static_assert(uninitialized == optional(tv::none));
static_assert(uninitialized != optional(0_bi));
static_assert(tv::none == uninitialized);
static_assert(optional(tv::none) == uninitialized);
static_assert(optional(0_bi) != uninitialized);

constexpr auto initialized = optional(5_bi);

static_assert(initialized != tv::none);
static_assert(initialized != optional(tv::none));
static_assert(initialized == optional(5_bi));
static_assert(initialized != optional(4_bi));

static_assert(tv::none != initialized);
static_assert(optional(5_bi) == initialized);
static_assert(optional(1_bi) != initialized);

template<typename Optional>
constexpr auto check_empty_braces() {
	Optional op = {};
	BOUNDED_ASSERT(!op);
	op = {};
	BOUNDED_ASSERT(!op);
	return true;
}

template<typename T>
constexpr auto check_uncompressed_optional() {
	using type = bounded::integer<bounded::builtin_min_value<T>, bounded::builtin_max_value<T>>;
	static_assert(
		sizeof(type) < sizeof(tv::optional<type>),
		"Compressing an optional that should not be compressed."
	);
	check_empty_braces<tv::optional<type>>();
}
template<auto minimum, auto maximum>
constexpr auto check_compressed_optional() {
	using type = bounded::integer<minimum, maximum>;
	static_assert(
		sizeof(type) == sizeof(tv::optional<type>),
		"compressed_optional too big."
	);
	check_empty_braces<tv::optional<type>>();
}

template<typename integer_type>
constexpr auto check_integer_optional() {
	constexpr auto uninitialized_optional = tv::optional<integer_type>();
	static_assert(!uninitialized_optional, "Default constructor should leave uninitialized.");
	constexpr auto constexpr_optional_integer = tv::optional<integer_type>(integer_type(5_bi));
	static_assert(constexpr_optional_integer, "Value constructor should initialize optional.");
	static_assert(*constexpr_optional_integer == 5, "Value in an optional incorrect.");

	auto optional_integer = tv::optional<integer_type>(integer_type(4_bi));
	optional_integer = uninitialized_optional;
	BOUNDED_ASSERT(!optional_integer);

	optional_integer = integer_type(7_bi);
	BOUNDED_ASSERT(optional_integer);

	static_assert(std::same_as<decltype(*optional_integer), integer_type &>, "Incorrect type of *optional.");
	*optional_integer = integer_type(1_bi);
	BOUNDED_ASSERT(optional_integer);
	BOUNDED_ASSERT(*optional_integer == 1_bi);
	
	optional_integer = tv::none;
	BOUNDED_ASSERT(!optional_integer);

	check_empty_braces<tv::optional<integer_type>>();
}

template<typename type>
constexpr auto check_non_trivial_optional() {
	auto uninitialized_optional = tv::optional<type>();
	BOUNDED_ASSERT(!uninitialized_optional);
	auto optional_int = tv::optional<type>(1);
	BOUNDED_ASSERT(optional_int);
	BOUNDED_ASSERT(*optional_int == 1);

	optional_int = uninitialized_optional;
	BOUNDED_ASSERT(!optional_int);

	optional_int = type(2);
	BOUNDED_ASSERT(optional_int);
	optional_int = 3;
	BOUNDED_ASSERT(optional_int);
	BOUNDED_ASSERT(*optional_int == 3);
	BOUNDED_ASSERT(*optional_int != 4);

	static_assert(std::same_as<decltype(*optional_int), type &>, "Incorrect type of *optional.");
	*optional_int = type(5);
	BOUNDED_ASSERT(optional_int);
	BOUNDED_ASSERT(*optional_int == 5);
	
	optional_int = tv::none;
	BOUNDED_ASSERT(!optional_int);

	check_empty_braces<tv::optional<type>>();
}

constexpr auto check_optional() {
	check_compressed_optional<1, 10>();
	check_compressed_optional<-50, 127>();
	check_uncompressed_optional<std::uint8_t>();
	check_uncompressed_optional<int>();
	check_uncompressed_optional<unsigned>();
	check_uncompressed_optional<numeric_traits::max_signed_t>();

	check_integer_optional<int>();
	check_integer_optional<bounded::integer<1, 10>>();
	check_non_trivial_optional<bounded_test::integer>();

	constexpr auto original = tv::make_optional(0_bi);
	constexpr auto copy = original;
	static_assert(original == copy);

	return true;
}

static_assert(check_optional());

static_assert(!tv::optional<int const &>());
static_assert(!tv::optional<int &>());

static_assert(bounded::constructible_from<tv::optional<int const &>, int const &>);
static_assert(bounded::constructible_from<tv::optional<int const &>, int &&>);
static_assert(bounded::constructible_from<tv::optional<int const &>, int &>);
static_assert(!bounded::constructible_from<tv::optional<int &>, int const &>);
static_assert(!bounded::constructible_from<tv::optional<int &>, int &&>);
static_assert(bounded::constructible_from<tv::optional<int &>, int &>);
static_assert(!std::is_assignable_v<tv::optional<int &> &, int &>);

static_assert([] {
	auto const value = 5;
	auto optional_ref = tv::optional<int const &>(value);
	return std::addressof(*optional_ref) == std::addressof(value);
}());

static_assert([] {
	auto value = 5;
	auto optional_ref = tv::optional<int const &>(value);
	return std::addressof(*optional_ref) == std::addressof(value);
}());

static_assert([] {
	auto value = 5;
	auto optional_ref = tv::optional<int &>(value);
	return std::addressof(*optional_ref) == std::addressof(value);
}());

static_assert([] {
	auto value1 = 5;
	auto value2 = 5;
	auto optional_ref = tv::optional<int &>(value1);
	optional_ref.emplace(bounded::value_to_function(value2));
	return std::addressof(*optional_ref) == std::addressof(value2);
}());

static_assert(tv::optional<tv::optional<int>>() == tv::none);
static_assert(tv::optional<tv::optional<int>>(tv::optional<int>()) != tv::none);
static_assert(tv::optional<tv::optional<int>>(tv::optional<int>()) == tv::optional<int>());
static_assert(tv::optional<tv::optional<int>>(tv::optional<int>(3)) == tv::optional<int>(3));

struct two_spare_representations {
	int x;
	friend auto operator==(two_spare_representations, two_spare_representations) -> bool = default;
};

template<>
struct bounded::tombstone<two_spare_representations> {
	static constexpr auto make(auto const index) noexcept -> two_spare_representations {
		return two_spare_representations{index == 0_bi ? 12 : 19};
	}
	static constexpr auto index(two_spare_representations const & value) noexcept -> bounded::integer<-1, 1> {
		return
			BOUNDED_CONDITIONAL(value.x == 12, 0_bi,
			BOUNDED_CONDITIONAL(value.x == 19, 1_bi,
			-1_bi
		));
	}
};

static_assert(bounded::tombstone_traits<two_spare_representations>::spare_representations == 2_bi);

static_assert(bounded::tombstone_traits<tv::optional<two_spare_representations>>::spare_representations == 1_bi);
static_assert(bounded::tombstone_traits<tv::optional<tv::optional<two_spare_representations>>>::spare_representations == 0_bi);

static_assert(sizeof(tv::optional<two_spare_representations>) == sizeof(two_spare_representations));
static_assert(sizeof(tv::optional<tv::optional<two_spare_representations>>) == sizeof(two_spare_representations));
static_assert(sizeof(tv::optional<tv::optional<tv::optional<two_spare_representations>>>) > sizeof(two_spare_representations));

static_assert(tv::optional<two_spare_representations>() == tv::none);
static_assert(tv::optional<two_spare_representations>(two_spare_representations(3)) == two_spare_representations(3));

static_assert(tv::optional<tv::optional<two_spare_representations>>() == tv::none);

static_assert(tv::optional<tv::optional<two_spare_representations>>(tv::optional<two_spare_representations>()) != tv::none);
static_assert(tv::optional<tv::optional<two_spare_representations>>(tv::optional<two_spare_representations>()) == tv::optional<two_spare_representations>());
static_assert(*tv::optional<tv::optional<two_spare_representations>>(tv::optional<two_spare_representations>(two_spare_representations(3))) == two_spare_representations(3));

static_assert(sizeof(tv::optional<int>) == sizeof(tv::optional<tv::optional<int>>));