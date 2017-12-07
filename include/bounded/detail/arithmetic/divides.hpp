// Copyright David Stone 2017.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <bounded/detail/arithmetic/base.hpp>
#include <bounded/detail/arithmetic/unary_minus.hpp>
#include <bounded/detail/comparison.hpp>
#include <bounded/detail/minmax.hpp>

#include <bounded/detail/abs.hpp>

#include <limits>

namespace bounded {
namespace detail {

// Ignores divide by 0, caught by constexpr
template<auto lhs_, auto rhs_>
constexpr auto safer_divide(constant_t<lhs_> const lhs, constant_t<rhs_> const rhs) noexcept {
	constexpr auto max_signed = constant<std::numeric_limits<std::intmax_t>::max()>;
	constexpr auto min_signed = constant<std::numeric_limits<std::intmax_t>::min()>;
	static_assert(lhs <= -min_signed or rhs != constant<-1>);
	if constexpr (lhs > max_signed) {
		if constexpr (rhs > constant<0>) {
			return constant<static_cast<std::uintmax_t>(lhs_) / static_cast<std::uintmax_t>(rhs_)>;
		} else {
			return -constant<static_cast<std::uintmax_t>(lhs_) / static_cast<std::uintmax_t>(-rhs)>;
		}
	} else if constexpr (lhs == min_signed and min_signed != -max_signed and rhs == constant<-1>) {
		return -min_signed;
	} else {
		return constant<static_cast<std::intmax_t>(lhs_) / static_cast<std::intmax_t>(rhs_)>;
	}
}

constexpr auto divides_operator_range = [](auto const lhs_, auto const rhs_) noexcept {
	constexpr auto lhs = min_max(decltype(lhs_.min){}, decltype(lhs_.max){});
	constexpr auto rhs = min_max(decltype(rhs_.min){}, decltype(rhs_.max){});
	// If 1 falls within the range, that is the least positive divisor. The
	// other options are a range that are entirely positive, in which case I
	// want to return the least value, or the range is entirely negative or
	// zero, in which case I pick the greatest absolute value (which is the
	// minimum) so that the 'positive' divisor is not selected in a later step.
	// We can use similar logic for greatest_negative_divisor.
	constexpr auto conditional = [](auto condition, auto if_true, auto if_false) {
		if constexpr (condition) {
			static_cast<void>(if_false);
			return if_true;
		} else {
			static_cast<void>(if_true);
			return if_false;
		}
	};
	constexpr auto least_positive_divisor = conditional(std::bool_constant<rhs.min <= constant<1> and constant<1> <= rhs.max>{}, constant<1>, rhs.min);
	constexpr auto greatest_negative_divisor = conditional(std::bool_constant<rhs.min <= constant<-1> and constant<-1> <= rhs.max>{}, constant<-1>, rhs.max);

	constexpr auto g0 = safer_divide(lhs.min, least_positive_divisor);
	constexpr auto g1 = safer_divide(lhs.min, greatest_negative_divisor);
	constexpr auto g2 = safer_divide(lhs.max, least_positive_divisor);
	constexpr auto g3 = safer_divide(lhs.max, greatest_negative_divisor);

	return min_max(
		cast_to_builtin(min(g0, g1, g2, g3)),
		cast_to_builtin(max(g0, g1, g2, g3))
	);
};

}	// namespace detail

BOUNDED_INTEGER_OPERATOR_OVERLOADS(/, detail::divides_operator_range)

}	// namespace bounded

