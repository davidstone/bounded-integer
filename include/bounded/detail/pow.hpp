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

namespace bounded {
namespace detail {

constexpr auto pow_impl(max_unsigned_t base, max_unsigned_t exponent) {
	if (exponent == 0) {
		BOUNDED_ASSERT(base != 0);
		return max_unsigned_t(1);
	}
	while (exponent % 2 == 0) {
		base *= base;
		exponent /= 2;
	}
	auto result = base;
	for (auto n = max_unsigned_t(0); n != exponent - 1; ++n) {
		auto const original_result = result;
		result *= base;
		BOUNDED_ASSERT(result >= original_result);
	}
	return result;
}

} // namespace detail

// TODO: Implement this efficiently
// TODO: support negative bases
constexpr auto pow(bounded_integer auto const base, bounded_integer auto const exponent) {
	using base_t = decltype(base);
	using exponent_t = decltype(exponent);
	static_assert(min_value<exponent_t> >= constant<0>);
	static_assert(min_value<base_t> >= constant<0>);
	return integer<
		detail::normalize<::bounded::detail::pow_impl(detail::builtin_min_value<base_t>, detail::builtin_min_value<exponent_t>)>,
		detail::normalize<::bounded::detail::pow_impl(detail::builtin_max_value<base_t>, detail::builtin_max_value<exponent_t>)>
	>(::bounded::detail::pow_impl(base.value(), exponent.value()));
}

} // namespace bounded
