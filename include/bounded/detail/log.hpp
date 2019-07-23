// Copyright David Stone 2017.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <bounded/detail/arithmetic/common.hpp>
#include <bounded/detail/basic_numeric_limits.hpp>
#include <bounded/detail/class.hpp>
#include <bounded/detail/is_bounded_integer.hpp>
#include <bounded/detail/max_builtin.hpp>

namespace bounded {
namespace detail {

constexpr inline auto as_unsigned = [](auto const value) noexcept {
	return static_cast<promoted_unsigned<decltype(value)>>(value);
};

template<typename Integer, typename Base>
constexpr auto log_impl(Integer value, Base const base) noexcept {
	auto sum = 0;
	while (value >= base) {
		value /= base;
		++sum;
	}
	return sum;
}

}	// namespace detail

template<auto minimum, auto maximum, typename overflow_policy, typename Base> requires is_bounded_integer<Base>
constexpr auto log(integer<minimum, maximum, overflow_policy> const value, Base const base) noexcept {
	static_assert(base > constant<1>, "Negative bases not currently supported.");
	static_assert(value > constant<0>, "The log of a negative number or zero is undefined.");
	using result_type = integer<
		detail::log_impl(static_cast<detail::max_unsigned_t>(minimum), static_cast<detail::max_unsigned_t>(basic_numeric_limits<Base>::max())),
		detail::log_impl(static_cast<detail::max_unsigned_t>(maximum), static_cast<detail::max_unsigned_t>(basic_numeric_limits<Base>::min())),
		overflow_policy
	>;
	return result_type(detail::log_impl(detail::as_unsigned(value.value()), detail::as_unsigned(base.value())), non_check);
}

}	// namespace bounded

namespace std {
// This overload is required for std::sort to work on gcc with iterators whose
// difference_type is a bounded::integer. It is not guaranteed to continue to be
// supported. It returns a built-in integer that is the log2 of the number
template<auto minimum, auto maximum, typename overflow_policy>
constexpr auto __lg(bounded::integer<minimum, maximum, overflow_policy> const value) BOUNDED_NOEXCEPT_DECLTYPE(
	__lg(value.value())
)

}	// namespace std
