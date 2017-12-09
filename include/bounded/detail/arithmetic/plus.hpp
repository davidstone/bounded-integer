// Copyright David Stone 2017.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <bounded/detail/arithmetic/base.hpp>
#include <bounded/detail/basic_numeric_limits.hpp>
#include <bounded/detail/comparison.hpp>
#include <bounded/detail/max_builtin.hpp>

namespace bounded {
namespace detail {

template<auto lhs_, auto rhs_>
constexpr auto safer_add(constant_t<lhs_> const lhs, constant_t<rhs_> const rhs) noexcept {
	constexpr auto modulo_equivalent_value = constant<static_cast<max_unsigned_t>(lhs_) + static_cast<max_unsigned_t>(rhs_)>;
	constexpr auto max_signed = constant<basic_numeric_limits<max_signed_t>::max()>;
	if constexpr ((lhs_ >= 0 and rhs_ >= 0) or lhs > max_signed or rhs > max_signed) {
		static_assert(
			(rhs_ < 0 or modulo_equivalent_value >= lhs) and (lhs_ < 0 or modulo_equivalent_value >= rhs),
			"Overflow in calculation of bounds."
		);
		using result_type = std::conditional_t<(modulo_equivalent_value > max_signed), max_unsigned_t, max_signed_t>;
		return static_cast<result_type>(modulo_equivalent_value);
	} else {
		// To enter this case, we know that at least one of lhs and rhs is
		// negative. If at least one of them is positive, addition cannot
		// underflow. If both are negative, we rely on modular arithmetic to
		// detect overflow.
		static_assert(
			lhs_ > 0 or rhs_ > 0 or modulo_equivalent_value > max_signed,
			"Underflow in calculation of bounds."
		);
		return static_cast<max_signed_t>(modulo_equivalent_value);
	}
}

constexpr auto plus_operator_range = [](auto const & lhs, auto const & rhs) noexcept {
	return min_max(
		safer_add(lhs.min, rhs.min),
		safer_add(lhs.max, rhs.max)
	);
};

}	// namespace detail

BOUNDED_INTEGER_OPERATOR_OVERLOADS(+, detail::plus_operator_range)

}	// namespace bounded

