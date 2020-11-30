// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <bounded/detail/arithmetic/base.hpp>
#include <bounded/detail/arithmetic/safe_abs.hpp>
#include <bounded/detail/arithmetic/unary_minus.hpp>
#include <bounded/detail/comparison.hpp>
#include <bounded/detail/max_builtin.hpp>
#include <bounded/detail/minmax.hpp>

namespace bounded {
namespace detail {

template<bool condition>
constexpr auto conditional_function(auto if_true, auto if_false) {
	if constexpr (condition) {
		return if_true;
	} else {
		return if_false;
	}
};


// Ignores divide by 0, caught by constexpr
template<auto lhs, auto rhs>
constexpr auto safer_divide() {
	constexpr auto is_negative = lhs < 0 xor rhs < 0;
	constexpr auto positive_result = bounded::constant<safe_abs(lhs) / safe_abs(rhs)>;
	if constexpr (is_negative) {
		return (-positive_result).value();
	} else {
		return positive_result.value();
	}
}

constexpr auto divides_operator_range = [](auto const lhs_, auto const rhs_) {
	constexpr auto lhs = min_max{lhs_.min, lhs_.max};
	constexpr auto rhs = min_max{rhs_.min, rhs_.max};
	// If 1 falls within the range, that is the least positive divisor. The
	// other options are a range that are entirely positive, in which case I
	// want to return the least value, or the range is entirely negative or
	// zero, in which case I pick the greatest absolute value (which is the
	// minimum) so that the 'positive' divisor is not selected in a later step.
	// We can use similar logic for greatest_negative_divisor.
	constexpr auto least_positive_divisor = ::bounded::detail::conditional_function<rhs.min <= constant<1> and constant<1> <= rhs.max>(1, rhs.min.value());
	constexpr auto greatest_negative_divisor = ::bounded::detail::conditional_function<rhs.min <= constant<-1> and constant<-1> <= rhs.max>(-1, rhs.max.value());

	constexpr auto g0 = safer_divide<lhs.min.value(), least_positive_divisor>();
	constexpr auto g1 = safer_divide<lhs.min.value(), greatest_negative_divisor>();
	constexpr auto g2 = safer_divide<lhs.max.value(), least_positive_divisor>();
	constexpr auto g3 = safer_divide<lhs.max.value(), greatest_negative_divisor>();

	return min_max{
		safe_min(g0, g1, g2, g3),
		safe_max(g0, g1, g2, g3)
	};
};

// #include <functional> is really expensive, and we just need this one part
inline constexpr auto divides = [](auto const lhs, auto const rhs) {
	return lhs / rhs;
};

}	// namespace detail

constexpr auto operator/(bounded_integer auto const lhs, bounded_integer auto const rhs) {
	return detail::operator_overload(lhs, rhs, detail::divides, detail::divides_operator_range);
}

}	// namespace bounded

