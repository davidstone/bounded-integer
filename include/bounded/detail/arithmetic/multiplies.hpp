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

namespace bounded {
namespace detail {

template<auto lhs, auto rhs>
constexpr auto safer_multiply(constant_t<lhs> const &, constant_t<rhs> const &) {
	// If both are positive, the result is positive and I can safely use
	// unsigned integers. If one is positive and one is negative, the result is
	// negative and I can use signed integers. Overflow will be caught by the
	// constexpr evaluation rules. If both are negative, I can cast them both to
	// unsigned and the result will be positive and thus fit in unsigned. The
	// conversion to unsigned will preserve the value under multiplication.
	if constexpr ((lhs < 0) xor (rhs < 0)) {
		constexpr auto signed_max = max_value<max_signed_t>;
		static_assert(lhs <= signed_max or rhs == 0);
		static_assert(rhs <= signed_max or lhs == 0);
		return static_cast<max_signed_t>(lhs) * static_cast<max_signed_t>(rhs);
	} else {
		constexpr auto result = static_cast<max_unsigned_t>(lhs) * static_cast<max_unsigned_t>(rhs);
		static_assert((safe_abs(lhs) <= result and safe_abs(rhs) <= result) or (lhs == 0 or rhs == 0), "Integer overflow in multiplication.");
		return result;
	}
}

// #include <functional> is really expensive, and we just need this one part
auto multiplies = [](auto const lhs, auto const rhs) {
	return lhs * rhs;
};

}	// namespace detail

template<
	auto lhs_min, auto lhs_max, typename lhs_policy,
	auto rhs_min, auto rhs_max, typename rhs_policy
>
constexpr auto operator*(
	integer<lhs_min, lhs_max, lhs_policy> const lhs_,
	integer<rhs_min, rhs_max, rhs_policy> const rhs_
) {
	return detail::operator_overload(lhs_, rhs_, detail::multiplies, [](auto const lhs, auto const rhs) {
		auto p0 = detail::safer_multiply(lhs.min, rhs.min);
		auto p1 = detail::safer_multiply(lhs.min, rhs.max);
		auto p2 = detail::safer_multiply(lhs.max, rhs.min);
		auto p3 = detail::safer_multiply(lhs.max, rhs.max);
		return detail::min_max{
			detail::safe_min(p0, p1, p2, p3),
			detail::safe_max(p0, p1, p2, p3)
		};
	});
}

}	// namespace bounded
