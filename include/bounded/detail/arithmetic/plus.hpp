// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <bounded/detail/arithmetic/base.hpp>
#include <bounded/detail/comparison.hpp>

#include <numeric_traits/int128.hpp>
#include <numeric_traits/min_max_value.hpp>

namespace bounded {
namespace detail {

template<auto lhs, auto rhs>
constexpr auto safer_add() {
	constexpr auto modulo_equivalent_value = static_cast<numeric_traits::max_unsigned_t>(lhs) + static_cast<numeric_traits::max_unsigned_t>(rhs);
	if constexpr (lhs >= 0 and rhs >= 0) {
		static_assert(
			modulo_equivalent_value >= static_cast<numeric_traits::max_unsigned_t>(lhs) and modulo_equivalent_value >= static_cast<numeric_traits::max_unsigned_t>(rhs),
			"Overflow in calculation of bounds."
		);
		return modulo_equivalent_value;
	} else if constexpr (lhs > max_signed or rhs > max_signed) {
		return modulo_equivalent_value;
	} else {
		static_assert(
			lhs > 0 or rhs > 0 or modulo_equivalent_value > max_signed,
			"Overflow in calculation of bounds."
		);
		return static_cast<numeric_traits::max_signed_t>(modulo_equivalent_value);
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
			::bounded::detail::safer_add<lhs.min.value(), rhs.min.value()>(),
			::bounded::detail::safer_add<lhs.max.value(), rhs.max.value()>()
		};
	});
}

}	// namespace bounded

