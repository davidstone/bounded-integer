// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <bounded/detail/arithmetic/base.hpp>
#include <bounded/detail/arithmetic/safe_abs.hpp>
#include <bounded/detail/max_builtin.hpp>
#include <bounded/detail/min_max_value.hpp>
#include <bounded/detail/minmax.hpp>
#include <bounded/detail/safe_extreme.hpp>

namespace bounded {
namespace detail {

template<auto lhs, auto rhs>
constexpr auto safer_multiply(constant_t<lhs> const &, constant_t<rhs> const &) {
	constexpr auto negative = (lhs < 0) xor (rhs < 0);
	constexpr auto positive_lhs = safe_abs(lhs);
	constexpr auto positive_rhs = safe_abs(rhs);
	constexpr auto positive_result = positive_lhs * positive_rhs;
	static_assert(
		lhs == 0 or positive_result / positive_lhs == positive_rhs,
		"Multiplication requires a larger type than currently supported."
	);
	if constexpr (negative) {
		static_assert(
			positive_result <= -static_cast<max_unsigned_t>(min_value<max_signed_t>),
			"Multiplication requires a larger type than currently supported."
		);
		return static_cast<max_signed_t>(-positive_result);
	} else {
		return positive_result;
	}
}

// #include <functional> is really expensive, and we just need this one part
inline constexpr auto multiplies = [](auto const lhs, auto const rhs) {
	return lhs * rhs;
};

}	// namespace detail

constexpr auto operator*(bounded_integer auto const lhs_, bounded_integer auto const rhs_) {
	return detail::modulo_equivalent_operator_overload(lhs_, rhs_, detail::multiplies, [](auto const lhs, auto const rhs) {
		constexpr auto p0 = detail::safer_multiply(lhs.min, rhs.min);
		constexpr auto p1 = detail::safer_multiply(lhs.min, rhs.max);
		constexpr auto p2 = detail::safer_multiply(lhs.max, rhs.min);
		constexpr auto p3 = detail::safer_multiply(lhs.max, rhs.max);
		return detail::min_max{
			detail::safe_min(p0, p1, p2, p3),
			detail::safe_max(p0, p1, p2, p3)
		};
	});
}

}	// namespace bounded
