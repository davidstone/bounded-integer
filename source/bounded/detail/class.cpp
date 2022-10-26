// Copyright David Stone 2017.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <bounded/detail/class.hpp>

#include <bounded/detail/comparison.hpp>
#include <bounded/detail/conditional.hpp>

#include <numeric_traits/min_max_value.hpp>

#include "../homogeneous_equals.hpp"
#include "../../test_assert.hpp"

namespace {

using type1 = bounded::integer<1, 5>;
using type2 = bounded::integer<3, 10>;
static_assert(std::same_as<std::common_type_t<type1, type2>, bounded::integer<1, 10>>);
using type3 = bounded::integer<-5, -5>;
static_assert(std::same_as<std::common_type_t<type1, type2, type3>, bounded::integer<-5, 10>>);
using type4 = bounded::integer<0, 0>;
static_assert(std::same_as<std::common_type_t<type1, type2, type3, type4>, bounded::integer<-5, 10>>);

static_assert(bounded::detail::overlapping_integer<bounded::integer<0, 0>, 0, 0>, "Type should overlap its own range.");
static_assert(!bounded::detail::overlapping_integer<bounded::integer<0, 0>, 1, 1>, "Type should not overlap a disjoint range.");

static_assert(bounded::detail::bounded_by_range<bounded::integer<0, 0>, -1, 1>);
static_assert(!bounded::detail::bounded_by_range<bounded::integer<0, 1>, 0, 0>);

static_assert(std::is_empty_v<bounded::constant_t<0>>);
static_assert(std::is_empty_v<bounded::constant_t<1>>);
static_assert(std::is_empty_v<bounded::constant_t<-1>>);
static_assert(std::is_empty_v<bounded::constant_t<bounded::builtin_min_value<std::intmax_t>>>);
static_assert(std::is_empty_v<bounded::constant_t<bounded::builtin_min_value<bounded::detail::max_signed_t>>>);
static_assert(std::is_empty_v<bounded::constant_t<bounded::builtin_max_value<bounded::detail::max_signed_t>>>);
static_assert(std::is_empty_v<bounded::constant_t<bounded::builtin_max_value<bounded::detail::max_unsigned_t>>>);

static_assert(!std::is_convertible_v<bool, bounded::integer<0, 1>>);
static_assert(bounded::constructible_from<bounded::integer<0, 1>, bool>);
static_assert(!bounded::constructible_from<bounded::integer<0, 0>, bool>);

namespace check_constructibility {
	constexpr auto min = numeric_traits::min_value<int>;
	constexpr auto max = numeric_traits::max_value<int>;
	using type = bounded::integer<min, max>;
	static_assert(
		bounded::detail::overlapping_integer<type, min, max>,
		"Bounds of type do not overlap its own range."
	);

	static_assert(
		std::is_convertible_v<int, type>,
		"Cannot convert integer type to bounded::integer with same range."
	);
	static_assert(
		bounded::constructible_from<type, type, bounded::unchecked_t>,
		"Cannot construct a type from itself with unchecked_t."
	);

	static_assert(
		!std::is_convertible_v<bool, type>,
		"Should not be able to convert a bool to a bounded::integer."
	);
	static_assert(
		bounded::constructible_from<type, bool>,
		"Should be able to construct a bounded::integer from a bool."
	);
}

static_assert(homogeneous_equals(
	BOUNDED_CONDITIONAL(true, bounded::constant<7>, bounded::constant<9>),
	bounded::integer<7, 9>(bounded::constant<7>)
));

constexpr auto check_assignment() {
	bounded::integer<0, 10> x(bounded::constant<5>);
	static_assert(!std::is_assignable<decltype((x)), bounded::constant_t<11>>{}, "Should not be assignable.");
	x = bounded::integer<10, 11>(bounded::constant<10>);
	BOUNDED_TEST(x == bounded::constant<10>);
	return true;
}

static_assert(check_assignment());


enum class bounded_enum{};

} // namespace

template<>
constexpr auto numeric_traits::min_value<bounded_enum> = 0;

template<>
constexpr auto numeric_traits::max_value<bounded_enum> = 0;

namespace {

enum unscoped_enum : int {};
static_assert(!bounded::constructible_from<bounded::integer<0, 10>, unscoped_enum>);
static_assert(!std::is_convertible_v<unscoped_enum, bounded::integer<0, 10>>);

enum class scoped_enum {};
static_assert(!bounded::constructible_from<bounded::integer<0, 10>, scoped_enum>);
static_assert(!std::is_convertible_v<scoped_enum, bounded::integer<0, 10>>);

static_assert(bounded::constructible_from<bounded::integer<0, 10>, bounded_enum>);
static_assert(!std::is_convertible_v<bounded_enum, bounded::integer<0, 10>>);
static_assert(bounded::integer(bounded_enum{}) == bounded::constant<0>);
static_assert(bounded::constant<bounded_enum{}> == bounded::constant<0>);

enum class bounded_integer_enum{};

} // namespace

template<>
constexpr auto numeric_traits::min_value<bounded_integer_enum> = bounded::constant<0>;

template<>
constexpr auto numeric_traits::max_value<bounded_integer_enum> = bounded::constant<0>;

namespace {

static_assert(!std::is_convertible_v<bounded_integer_enum, bounded::integer<0, 0>>);
static_assert(bounded::integer(bounded_integer_enum{}) == bounded::constant<0>);
static_assert(bounded::constant<bounded_integer_enum{}> == bounded::constant<0>);

enum class enum_bounded_enum{};

} // namespace

template<>
constexpr auto numeric_traits::min_value<enum_bounded_enum> = enum_bounded_enum();

template<>
constexpr auto numeric_traits::max_value<enum_bounded_enum> = enum_bounded_enum();

namespace {

static_assert(!std::is_convertible_v<enum_bounded_enum, bounded::integer<0, 0>>);
static_assert(bounded::integer(enum_bounded_enum{}) == bounded::constant<0>);
static_assert(bounded::constant<enum_bounded_enum{}> == bounded::constant<0>);

} // namespace
