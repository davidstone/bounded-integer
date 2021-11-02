// Copyright David Stone 2020.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <bounded/assert.hpp>
#include <bounded/detail/arithmetic/multiplies.hpp>
#include <bounded/detail/cast.hpp>
#include <bounded/detail/class.hpp>
#include <bounded/detail/max_builtin.hpp>

#include <numeric_traits/min_max_value.hpp>

namespace bounded {
namespace detail {

constexpr auto pow_impl(max_unsigned_t base, max_unsigned_t exponent) {
	if (exponent == 0) {
		BOUNDED_ASSERT(base != 0);
		return max_unsigned_t(1);
	}
	while (exponent % 2 == 0) {
		auto const original_base = base;
		base *= base;
		BOUNDED_ASSERT(original_base == 0 or base / original_base == original_base);
		exponent /= 2;
	}
	auto result = base;
	for (auto n = max_unsigned_t(0); n != exponent - 1; ++n) {
		auto const original_result = result;
		result *= base;
		BOUNDED_ASSERT(base == 0 or result / base == original_result);
	}
	return result;
}

} // namespace detail

// TODO: Implement this efficiently
// TODO: support negative bases
constexpr auto pow(bounded_integer auto const base, bounded_integer auto const exponent) {
	using base_t = decltype(base);
	using exponent_t = decltype(exponent);
	static_assert(numeric_traits::min_value<exponent_t> >= constant<0>);
	static_assert(numeric_traits::min_value<base_t> >= constant<0>);
	return integer<
		normalize<::bounded::detail::pow_impl(builtin_min_value<base_t>, builtin_min_value<exponent_t>)>,
		normalize<::bounded::detail::pow_impl(builtin_max_value<base_t>, builtin_max_value<exponent_t>)>
	>(::bounded::detail::pow_impl(base.value(), exponent.value()), non_check);
}

} // namespace bounded
