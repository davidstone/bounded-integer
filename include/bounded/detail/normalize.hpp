// Copyright David Stone 2019.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <bounded/detail/max_builtin.hpp>
#include <bounded/detail/min_max_value.hpp>

#include <type_traits>

namespace bounded {
namespace detail {

constexpr auto must_use_unsigned(auto const value) {
	return value > 0 and static_cast<max_unsigned_t>(value) > static_cast<max_unsigned_t>(max_value<max_signed_t>);
}

template<typename T>
constexpr auto fits_in_int(T const value) {
	return (value < static_cast<T>(0)) ?
		value >= static_cast<T>(min_value<int>) :
		value <= static_cast<T>(max_value<int>);
}

template<auto value>
inline constexpr auto normalize = static_cast<
	std::conditional_t<fits_in_int(+value), int,
	std::conditional_t<must_use_unsigned(value), max_unsigned_t,
	max_signed_t
>>>(value);

template<auto value> requires std::is_enum_v<decltype(value)>
inline constexpr auto normalize<value> = normalize<static_cast<std::underlying_type_t<decltype(value)>>(value)>;

} // namespace detail
} // namespace bounded
