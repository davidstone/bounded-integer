// Copyright David Stone 2017.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <bounded/detail/arithmetic/operators.hpp>
#include <bounded/detail/class.hpp>
#include <bounded/detail/is_bounded_integer.hpp>
#include <bounded/detail/max_builtin.hpp>

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

}	// namespace detail

template<bounded_integer Value, bounded_integer Base>
constexpr auto log(Value const value, Base const base) {
	static_assert(base > constant<1>, "Negative bases not currently supported.");
	static_assert(value > constant<0>, "The log of a negative number or zero is undefined.");
	using result_type = integer<
		detail::log_impl(static_cast<detail::max_unsigned_t>(builtin_min_value<Value>), static_cast<detail::max_unsigned_t>(numeric_traits::max_value<Base>)),
		detail::log_impl(static_cast<detail::max_unsigned_t>(builtin_max_value<Value>), static_cast<detail::max_unsigned_t>(numeric_traits::min_value<Base>))
	>;
	return result_type(detail::log_impl(detail::as_unsigned(value.value()), detail::as_unsigned(base.value())), non_check);
}

}	// namespace bounded

namespace std {
// This overload is required for std::sort to work on gcc with iterators whose
// difference_type is a bounded::integer. It is not guaranteed to continue to be
// supported. It returns a built-in integer that is the log2 of the number
constexpr auto __lg(bounded::bounded_integer auto const value) {
	return __lg(value.value());
}

}	// namespace std
