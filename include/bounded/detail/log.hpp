// Copyright David Stone 2017.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <bounded/detail/arithmetic/operators.hpp>
#include <bounded/detail/class.hpp>
#include <bounded/detail/is_bounded_integer.hpp>

#include <numeric_traits/int128.hpp>
#include <numeric_traits/min_max_value.hpp>

namespace bounded {
namespace detail {

constexpr auto log_impl(auto value, auto const base) {
	auto sum = 0;
	while (value >= base) {
		value /= base;
		++sum;
	}
	return sum;
}

constexpr inline auto as_unsigned = [](auto const value) {
	return static_cast<numeric_traits::promoted_unsigned<decltype(value)>>(value);
};

} // namespace detail

// TODO: It's useful for 0 to return 0, but that's not `log`.
template<bounded_integer Value, bounded_integer Base>
constexpr auto log(Value const value, Base const base) {
	static_assert(base > constant<1>, "Negative bases not currently supported.");
	static_assert(numeric_traits::min_value<Value> >= constant<0>, "The log of a negative number is undefined.");
	using result_type = integer<
		detail::log_impl(static_cast<numeric_traits::max_unsigned_t>(builtin_min_value<Value>), static_cast<numeric_traits::max_unsigned_t>(numeric_traits::max_value<Base>)),
		detail::log_impl(static_cast<numeric_traits::max_unsigned_t>(builtin_max_value<Value>), static_cast<numeric_traits::max_unsigned_t>(numeric_traits::min_value<Base>))
	>;
	return result_type(detail::log_impl(detail::as_unsigned(value.value()), detail::as_unsigned(base.value())), unchecked);
}

} // namespace bounded
