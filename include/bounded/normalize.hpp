// Copyright David Stone 2019.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <bounded/detail/is_bounded_integer.hpp>

#include <numeric_traits/int128.hpp>
#include <numeric_traits/min_max_value.hpp>

#include <type_traits>
#include <utility>

namespace bounded {
namespace detail {

template<typename T>
constexpr auto as_builtin_integer(T const x) {
	if constexpr (bounded_integer<T>) {
		return x.value();
	} else if constexpr (is_integral_constant_of_integral<T>) {
		return x.get();
	} else if constexpr (std::is_enum_v<T>) {
		return std::to_underlying(x);
	} else {
		static_assert(builtin_integer<T> or std::same_as<T, bool>);
		return x;
	}
}

constexpr auto must_use_unsigned(auto const value) {
	return value > 0 and static_cast<numeric_traits::max_unsigned_t>(value) > static_cast<numeric_traits::max_unsigned_t>(numeric_traits::max_value<numeric_traits::max_signed_t>);
}

template<typename T>
constexpr auto fits_in_int(T const value) {
	return (value < static_cast<T>(0)) ?
		value >= static_cast<T>(numeric_traits::min_value<int>) :
		value <= static_cast<T>(numeric_traits::max_value<int>);
}

} // namespace detail

template<auto value>
inline constexpr auto normalize = static_cast<
	std::conditional_t<detail::fits_in_int(+bounded::detail::as_builtin_integer(value)), int,
	std::conditional_t<detail::must_use_unsigned(bounded::detail::as_builtin_integer(value)), numeric_traits::max_unsigned_t,
	numeric_traits::max_signed_t
>>>(value);

} // namespace bounded
