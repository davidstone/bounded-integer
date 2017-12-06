// Copyright David Stone 2017.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <bounded/detail/arithmetic/base.hpp>
#include <bounded/detail/arithmetic/safe_abs.hpp>

#include <bounded/detail/minmax.hpp>

namespace bounded {
namespace detail {

template<auto lhs, auto rhs>
constexpr auto safer_multiply(constant_t<lhs> const &, constant_t<rhs> const &) noexcept {
	// If both are positive, the result is positive and I can safely use
	// unsigned integers. If one is positive and one is negative, the result is
	// negative and I can use signed integers. Overflow will be caught by the
	// constexpr evaluation rules. If both are negative, I can cast them both to
	// unsigned and the result will be positive and thus fit in unsigned. The
	// conversion to unsigned will preserve the value under multiplication.
	constexpr auto signed_max = std::numeric_limits<std::intmax_t>::max();
	if constexpr ((lhs < 0) xor (rhs < 0)) {
		static_assert(lhs <= signed_max or rhs == 0);
		static_assert(rhs <= signed_max or lhs == 0);
		return static_cast<std::intmax_t>(lhs) * static_cast<std::intmax_t>(rhs);
	} else {
		constexpr auto result = static_cast<std::uintmax_t>(lhs) * static_cast<std::uintmax_t>(rhs);
		static_assert((safe_abs(lhs) <= result and safe_abs(rhs) <= result) or (lhs == 0 or rhs == 0), "Integer overflow in multiplication.");
		using result_type = std::conditional_t<result <= signed_max, std::intmax_t, std::uintmax_t>;
		return static_cast<result_type>(result);
	}
}

constexpr auto multiplies_operator_range = [](auto const lhs, auto const rhs) noexcept {
	auto p0 = safer_multiply(lhs.min, rhs.min);
	auto p1 = safer_multiply(lhs.min, rhs.max);
	auto p2 = safer_multiply(lhs.max, rhs.min);
	auto p3 = safer_multiply(lhs.max, rhs.max);
	return min_max(
		min(p0, p1, p2, p3),
		max(p0, p1, p2, p3)
	);
};

}	// namespace detail

BOUNDED_INTEGER_OPERATOR_OVERLOADS(*, detail::multiplies_operator_range)

}	// namespace bounded

