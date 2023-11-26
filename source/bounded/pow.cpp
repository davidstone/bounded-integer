// Copyright David Stone 2020.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

module;

#include <bounded/assert.hpp>

export module bounded.pow;

import bounded.arithmetic.multiplies;
import bounded.bounded_integer;
import bounded.builtin_min_max_value;
import bounded.cast;
import bounded.comparison;
import bounded.integer;
import bounded.normalize;
import bounded.unchecked;

import numeric_traits;

namespace bounded {

constexpr auto pow_impl(numeric_traits::max_unsigned_t base, numeric_traits::max_unsigned_t exponent) {
	if (exponent == 0) {
		BOUNDED_ASSERT(base != 0);
		return numeric_traits::max_unsigned_t(1);
	}
	while (exponent % 2 == 0) {
		auto const original_base = base;
		base *= base;
		BOUNDED_ASSERT(original_base == 0 or base / original_base == original_base);
		exponent /= 2;
	}
	auto result = base;
	for (auto n = numeric_traits::max_unsigned_t(0); n != exponent - 1; ++n) {
		auto const original_result = result;
		result *= base;
		BOUNDED_ASSERT(base == 0 or result / base == original_result);
	}
	return result;
}

// TODO: Implement this efficiently
// TODO: support negative bases
export constexpr auto pow(bounded_integer auto const base, bounded_integer auto const exponent) {
	using base_t = decltype(base);
	using exponent_t = decltype(exponent);
	static_assert(numeric_traits::min_value<exponent_t> >= constant<0>);
	static_assert(numeric_traits::min_value<base_t> >= constant<0>);
	return integer<
		normalize<::bounded::pow_impl(builtin_min_value<base_t>, builtin_min_value<exponent_t>)>,
		normalize<::bounded::pow_impl(builtin_max_value<base_t>, builtin_max_value<exponent_t>)>
	>(::bounded::pow_impl(base.value(), exponent.value()), unchecked);
}

} // namespace bounded

static_assert(bounded::pow(bounded::constant<0>, bounded::constant<1>) == bounded::constant<0>);
static_assert(bounded::pow(bounded::constant<0>, bounded::constant<2>) == bounded::constant<0>);
static_assert(bounded::pow(bounded::constant<0>, bounded::constant<3>) == bounded::constant<0>);
static_assert(bounded::pow(bounded::constant<0>, bounded::constant<4>) == bounded::constant<0>);
static_assert(bounded::pow(bounded::constant<1>, bounded::constant<1>) == bounded::constant<1>);
static_assert(bounded::pow(bounded::constant<123>, bounded::constant<1>) == bounded::constant<123>);
static_assert(bounded::pow(bounded::constant<9235234>, bounded::constant<0>) == bounded::constant<1>);
static_assert(bounded::pow(bounded::constant<9>, bounded::constant<2>) == bounded::constant<81>);
static_assert(bounded::pow(bounded::constant<2>, bounded::constant<10>) == bounded::constant<1024>);
static_assert(bounded::pow(bounded::constant<3>, bounded::constant<5>) == bounded::constant<243>);
static_assert(bounded::pow(bounded::constant<4>, bounded::constant<4>) == bounded::constant<256>);
static_assert(bounded::pow(bounded::constant<2>, bounded::constant<63>) == bounded::constant<1ULL << 63ULL>);
