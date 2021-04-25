// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <bounded/detail/arithmetic/base.hpp>
#include <bounded/detail/comparison.hpp>
#include <bounded/detail/max_builtin.hpp>

#include <numeric_traits/min_max_value.hpp>

namespace bounded {
namespace detail {

template<auto lhs, auto rhs>
constexpr auto safer_add(constant_t<lhs>, constant_t<rhs>) {
	constexpr auto modulo_equivalent_value = static_cast<max_unsigned_t>(lhs) + static_cast<max_unsigned_t>(rhs);
	constexpr auto max_signed = numeric_traits::max_value<max_signed_t>;
	if constexpr ((lhs >= 0 and rhs >= 0) or lhs > max_signed or rhs > max_signed) {
		static_assert(
			(lhs < 0 or rhs < 0) or
			(modulo_equivalent_value >= static_cast<max_unsigned_t>(lhs) and modulo_equivalent_value >= static_cast<max_unsigned_t>(rhs)),
			"Overflow in calculation of bounds."
		);
		return modulo_equivalent_value;
	} else {
		// To enter this case, we know that at least one of lhs and rhs is
		// negative. If at least one of them is positive, addition cannot
		// underflow. If both are negative, we rely on modular arithmetic to
		// detect overflow.
		static_assert(
			lhs > 0 or rhs > 0 or modulo_equivalent_value > max_signed,
			"Underflow in calculation of bounds."
		);
		return static_cast<max_signed_t>(modulo_equivalent_value);
	}
}

// #include <functional> is really expensive, and we just need this one part
inline constexpr auto plus = [](auto const lhs, auto const rhs) {
	return lhs + rhs;
};

}	// namespace detail

constexpr auto operator+(bounded_integer auto const lhs_, bounded_integer auto const rhs_) {
	return detail::modulo_equivalent_operator_overload(lhs_, rhs_, detail::plus, [](auto const lhs, auto const rhs) {
		return detail::min_max{
			::bounded::detail::safer_add(lhs.min, rhs.min),
			::bounded::detail::safer_add(lhs.max, rhs.max)
		};
	});
}

}	// namespace bounded

