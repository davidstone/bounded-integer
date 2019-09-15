// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <bounded/detail/abs.hpp>
#include <bounded/detail/arithmetic/base.hpp>
#include <bounded/detail/arithmetic/unary_minus.hpp>
#include <bounded/detail/basic_numeric_limits.hpp>
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
template<auto lhs_, auto rhs_>
constexpr auto safer_divide(constant_t<lhs_> const lhs, constant_t<rhs_> const rhs) {
	constexpr auto max_signed = constant<basic_numeric_limits<max_signed_t>::max()>;
	constexpr auto min_signed = constant<basic_numeric_limits<max_signed_t>::min()>;
	static_assert(lhs <= -min_signed or rhs != constant<-1>);
	if constexpr (lhs > max_signed) {
		if constexpr (rhs > constant<0>) {
			return constant<static_cast<max_unsigned_t>(lhs_) / static_cast<max_unsigned_t>(rhs_)>;
		} else {
			return -constant<static_cast<max_unsigned_t>(lhs_) / static_cast<max_unsigned_t>(-rhs)>;
		}
	} else if constexpr (lhs == min_signed and min_signed != -max_signed and rhs == constant<-1>) {
		return -min_signed;
	} else {
		return constant<static_cast<max_signed_t>(lhs_) / static_cast<max_signed_t>(rhs_)>;
	}
}

constexpr auto divides_operator_range = [](auto const lhs_, auto const rhs_) {
	constexpr auto lhs = min_max{decltype(lhs_.min){}, decltype(lhs_.max){}};
	constexpr auto rhs = min_max{decltype(rhs_.min){}, decltype(rhs_.max){}};
	// If 1 falls within the range, that is the least positive divisor. The
	// other options are a range that are entirely positive, in which case I
	// want to return the least value, or the range is entirely negative or
	// zero, in which case I pick the greatest absolute value (which is the
	// minimum) so that the 'positive' divisor is not selected in a later step.
	// We can use similar logic for greatest_negative_divisor.
	constexpr auto least_positive_divisor = ::bounded::detail::conditional_function<rhs.min <= constant<1> and constant<1> <= rhs.max>(constant<1>, rhs.min);
	constexpr auto greatest_negative_divisor = ::bounded::detail::conditional_function<rhs.min <= constant<-1> and constant<-1> <= rhs.max>(constant<-1>, rhs.max);

	constexpr auto g0 = safer_divide(lhs.min, least_positive_divisor);
	constexpr auto g1 = safer_divide(lhs.min, greatest_negative_divisor);
	constexpr auto g2 = safer_divide(lhs.max, least_positive_divisor);
	constexpr auto g3 = safer_divide(lhs.max, greatest_negative_divisor);

	return min_max{
		min(g0, g1, g2, g3).value(),
		max(g0, g1, g2, g3).value()
	};
};

}	// namespace detail

template<
	auto lhs_min, auto lhs_max, typename lhs_policy,
	auto rhs_min, auto rhs_max, typename rhs_policy
>
constexpr auto operator/(
	integer<lhs_min, lhs_max, lhs_policy> const lhs,
	integer<rhs_min, rhs_max, rhs_policy> const rhs
) {
	return detail::operator_overload(lhs, rhs, std::divides{}, detail::divides_operator_range);
}

}	// namespace bounded

