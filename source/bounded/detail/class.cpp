// Copyright David Stone 2017.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <bounded/detail/class.hpp>

#include <bounded/detail/comparison.hpp>
#include <bounded/detail/conditional.hpp>
#include <bounded/detail/typedefs.hpp>

#include "../homogeneous_equals.hpp"
#include "../../test_assert.hpp"

namespace {

static_assert(bounded::detail::overlapping_integer<bounded::integer<0, 0>, 0, 0, bounded::null_policy>, "Type should overlap its own range.");
static_assert(!bounded::detail::overlapping_integer<bounded::integer<0, 0>, 1, 1, bounded::null_policy>, "Type should not overlap a disjoint range.");

static_assert(bounded::detail::bounded_by_range<bounded::integer<0, 0>, -1, 1, bounded::null_policy>);
static_assert(!bounded::detail::bounded_by_range<bounded::integer<0, 1>, 0, 0, bounded::null_policy>);

static_assert(std::is_empty<bounded::constant_t<0>>{});
static_assert(std::is_empty<bounded::constant_t<1>>{});
static_assert(std::is_empty<bounded::constant_t<-1>>{});
static_assert(std::is_empty<bounded::constant_t<bounded::min_value<std::intmax_t>>>{});

static_assert(!std::is_convertible_v<bool, bounded::integer<0, 1>>);
static_assert(std::is_constructible_v<bounded::integer<0, 1>, bool>);
static_assert(std::is_constructible_v<bounded::integer<0, 0>, bool>);

namespace check_constructibility {
	constexpr auto min = bounded::min_value<int>;
	constexpr auto max = bounded::max_value<int>;
	using type = bounded::integer<min, max, bounded::null_policy>;
	static_assert(
		bounded::detail::overlapping_integer<type, min, max, bounded::null_policy>,
		"Bounds of type do not overlap its own range."
	);

	static_assert(
		std::is_convertible_v<int, type>,
		"Cannot convert integer type to bounded::integer with same range."
	);
	static_assert(
		std::is_constructible_v<type, type, bounded::non_check_t>,
		"Cannot construct a type from itself with non_check_t."
	);

	static_assert(
		!std::is_convertible_v<bool, type>,
		"Should not be able to convert a bool to a bounded::integer."
	);
	static_assert(
		std::is_constructible_v<type, bool>,
		"Should be able to construct a bounded::integer from a bool."
	);
}

static_assert(homogeneous_equals(
	BOUNDED_CONDITIONAL(true, bounded::constant<7>, bounded::constant<9>),
	bounded::integer<7, 9>(7, bounded::non_check)
));

constexpr auto check_assignment() {
	bounded::integer<0, 10> x(5, bounded::non_check);
	static_assert(!std::is_assignable<decltype((x)), bounded::constant_t<11>>{}, "Should not be assignable.");
	x = bounded::integer<10, 11>(10, bounded::non_check);
	BOUNDED_TEST(x == bounded::constant<10>);

	bounded::clamped_integer<0, 10> y(5, bounded::non_check);
	y = bounded::constant<11>;
	BOUNDED_TEST(y == bounded::constant<10>);
	return true;
}

static_assert(check_assignment());


enum class bounded_enum{};

} // namespace
namespace bounded {

template<>
constexpr auto min_value<bounded_enum> = 0;

template<>
constexpr auto max_value<bounded_enum> = 0;

} // namespace bounded
namespace {

enum unscoped_enum : int {};
static_assert(std::is_constructible_v<bounded::integer<0, 10>, unscoped_enum>);
static_assert(!std::is_convertible_v<unscoped_enum, bounded::integer<0, 10>>);
static_assert(homogeneous_equals(
	bounded::integer(unscoped_enum{}),
	bounded::integer(static_cast<std::underlying_type_t<unscoped_enum>>(0))
));

enum class scoped_enum {};
static_assert(std::is_constructible_v<bounded::integer<0, 10>, scoped_enum>);
static_assert(!std::is_convertible_v<scoped_enum, bounded::integer<0, 10>>);
// constexpr auto b = bounded::integer(scoped_enum{});

static_assert(std::is_constructible_v<bounded::integer<0, 10>, bounded_enum>);
static_assert(!std::is_convertible_v<bounded_enum, bounded::integer<0, 10>>);
static_assert(bounded::integer(bounded_enum{}) == bounded::constant<0>);
static_assert(bounded::constant<bounded_enum{}> == bounded::constant<0>);

enum class bounded_integer_enum{};

} // namespace
namespace bounded {

template<>
constexpr auto min_value<bounded_integer_enum> = bounded::constant<0>;

template<>
constexpr auto max_value<bounded_integer_enum> = bounded::constant<0>;

} // namespace bounded
namespace {

static_assert(!std::is_convertible_v<bounded_integer_enum, bounded::integer<0, 0>>);
static_assert(bounded::integer(bounded_integer_enum{}) == bounded::constant<0>);
static_assert(bounded::constant<bounded_integer_enum{}> == bounded::constant<0>);

enum class enum_bounded_enum{};

} // namespace
namespace bounded {

template<>
constexpr auto min_value<enum_bounded_enum> = enum_bounded_enum();

template<>
constexpr auto max_value<enum_bounded_enum> = enum_bounded_enum();

} // namespace bounded
namespace {

static_assert(!std::is_convertible_v<enum_bounded_enum, bounded::integer<0, 0>>);
static_assert(bounded::integer(enum_bounded_enum{}) == bounded::constant<0>);
static_assert(bounded::constant<enum_bounded_enum{}> == bounded::constant<0>);

} // namespace
