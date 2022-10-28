// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <bounded/detail/arithmetic/base.hpp>
#include <bounded/detail/arithmetic/safe_abs.hpp>
#include <bounded/detail/minmax.hpp>
#include <bounded/detail/safe_extreme.hpp>

#include <numeric_traits/int128.hpp>
#include <numeric_traits/min_max_value.hpp>

namespace bounded {
namespace detail {

template<auto lhs, auto rhs>
constexpr auto safer_multiply(constant_t<lhs> const &, constant_t<rhs> const &) {
	constexpr auto negative = (lhs < 0) xor (rhs < 0);
	constexpr auto ulhs = static_cast<numeric_traits::max_unsigned_t>(lhs);
	constexpr auto urhs = static_cast<numeric_traits::max_unsigned_t>(rhs);
	constexpr auto modulo_equivalent_result = ulhs * urhs;
	if constexpr (negative) {
		if constexpr (lhs > 0) {
			static_assert(
				-modulo_equivalent_result / ulhs == -urhs,
				"Multiplication requires a larger type than currently supported."
			);
		} else if constexpr (rhs > 0) {
			static_assert(
				-modulo_equivalent_result / urhs == -ulhs,
				"Multiplication requires a larger type than currently supported."
			);
		}
		return static_cast<numeric_traits::max_signed_t>(modulo_equivalent_result);
	} else {
		if constexpr (lhs > 0) {
			static_assert(
				modulo_equivalent_result / ulhs == urhs,
				"Multiplication requires a larger type than currently supported."
			);
		} else if constexpr (lhs < 0) {
			static_assert(
				modulo_equivalent_result / -ulhs == -urhs,
				"Multiplication requires a larger type than currently supported."
			);
		}
		return modulo_equivalent_result;
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

// Not technically necessary, but improves compile times in some common cases
constexpr auto operator*(bounded_integer auto const lhs, bounded::constant_t<1>) {
	return lhs;
}
constexpr auto operator*(bounded::constant_t<1>, bounded_integer auto const rhs) {
	return rhs;
}
constexpr auto operator*(bounded::constant_t<1>, bounded::constant_t<1>) {
	return bounded::constant<1>;
}

}	// namespace bounded
