// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <bounded/detail/arithmetic/base.hpp>
#include <bounded/detail/arithmetic/safe_abs.hpp>
#include <bounded/detail/comparison.hpp>
#include <bounded/detail/minmax.hpp>
#include <bounded/detail/safe_extreme.hpp>

#include <numeric_traits/int128.hpp>

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
	constexpr auto is_negative = (lhs < 0) xor (rhs < 0);
	constexpr auto positive_result = safe_abs(lhs) / safe_abs(rhs);
	if constexpr (is_negative) {
		return static_cast<numeric_traits::max_signed_t>(-positive_result);
	} else {
		return positive_result;
	}
}

constexpr auto positive_divisor_division(auto const lhs, auto const rhs) {
	constexpr auto minimum_divisor = conditional_function<lhs.min.value() < 0>(rhs.min, rhs.max).value();
	constexpr auto maximum_divisor = conditional_function<lhs.max.value() < 0>(rhs.max, rhs.min).value();
	return min_max{safer_divide<lhs.min.value(), minimum_divisor>(), safer_divide<lhs.max.value(), maximum_divisor>()};
}
constexpr auto negative_divisor_division(auto const lhs, auto const rhs) {
	constexpr auto minimum_divisor = conditional_function<lhs.max.value() >= 0>(rhs.max, rhs.min).value();
	constexpr auto maximum_divisor = conditional_function<lhs.min.value() >= 0>(rhs.min, rhs.max).value();
	return min_max{safer_divide<lhs.max.value(), minimum_divisor>(), safer_divide<lhs.min.value(), maximum_divisor>()};
}

constexpr auto divides_operator_range = [](auto const lhs, auto const rhs) {
	static_assert(rhs.min != constant<0>);
	static_assert(rhs.max != constant<0>);
	if constexpr (rhs.min >= constant<1>) {
		return positive_divisor_division(lhs, rhs);
	} else if constexpr (rhs.max <= constant<-1>) {
		return negative_divisor_division(lhs, rhs);
	} else {
		constexpr auto positive = positive_divisor_division(lhs, min_max{constant<1>, rhs.max});
		constexpr auto negative = negative_divisor_division(lhs, min_max{rhs.min, constant<-1>});
		return min_max{
			safe_min(positive.min, negative.min),
			safe_max(positive.max, negative.max)
		};
	}
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

