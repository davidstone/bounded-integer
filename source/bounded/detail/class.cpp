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

static_assert(std::is_empty<bounded::constant_t<0>>{});
static_assert(std::is_empty<bounded::constant_t<1>>{});
static_assert(std::is_empty<bounded::constant_t<-1>>{});
static_assert(std::is_empty<bounded::constant_t<std::numeric_limits<std::intmax_t>::min()>>{});

constexpr auto check_constructibility() {
	constexpr auto min = std::numeric_limits<int>::min();
	constexpr auto max = std::numeric_limits<int>::max();
	using type = bounded::integer<min, max, bounded::null_policy>;
	static_assert(
		bounded::detail::type_overlaps_range<type>(min, max),
		"Bounds of type do not overlap its own range."
	);

	static_assert(
		bounded::detail::is_explicitly_constructible_from<bounded::null_policy, type>(min, max),
		"Type is not explicitly constructible from itself."
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
	
	return true;
}

static_assert(check_constructibility());

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

} // namespace bounded
namespace {

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

} // namespace
