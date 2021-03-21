// Copyright David Stone 2019, Malte Skarupke 2016.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <bounded/integer.hpp>

#include <cstdint>
#include <limits>
#include <type_traits>

namespace containers {
namespace detail {

struct unknown_floating_point;
template<typename T>
using float_to_unsigned =
	std::conditional_t<sizeof(T) == 4, std::uint32_t,
	std::conditional_t<sizeof(T) == 8, std::uint64_t,
	unknown_floating_point
>>;

} // namespace detail

// TODO: Delete bool overload, currently relying on implicit conversions
constexpr auto to_radix_sort_key(bool const value) {
	return value;
}
template<typename T> requires std::is_arithmetic_v<T>
constexpr auto to_radix_sort_key(T const value) {
	constexpr auto is_character_type =
		std::is_same_v<T, char> or
		std::is_same_v<T, char8_t> or
		std::is_same_v<T, char16_t> or
		std::is_same_v<T, char32_t> or
		std::is_same_v<T, wchar_t>;

	if constexpr (std::is_floating_point_v<T>) {
		static_assert(std::numeric_limits<T>::is_iec559);
		using unsigned_t = detail::float_to_unsigned<T>;
		static_assert(sizeof(T) == sizeof(unsigned_t));
		auto const u = __builtin_bit_cast(unsigned_t, value);
		constexpr auto sign_bit_position = std::numeric_limits<unsigned_t>::digits - 1U;
		auto const sign_bit = static_cast<unsigned_t>(-static_cast<std::make_signed_t<unsigned_t>>(u >> (sign_bit_position)));
		return u ^ (sign_bit | (static_cast<unsigned_t>(1U) << sign_bit_position));
	} else if constexpr (is_character_type) {
		return static_cast<std::make_unsigned_t<T>>(value);
	} else if constexpr (std::is_unsigned_v<T>) {
		return value;
	} else {
		static_assert(std::is_signed_v<T>);
		using unsigned_t = std::make_unsigned_t<T>;
		return static_cast<unsigned_t>(static_cast<unsigned_t>(value) + static_cast<unsigned_t>(bounded::min_value<T>));
	}
}

constexpr auto to_radix_sort_key(bounded::bounded_integer auto const value) {
	return to_radix_sort_key(value.value());
}

template<typename T> requires std::is_enum_v<T>
constexpr auto to_radix_sort_key(T const value) {
	return to_radix_sort_key(bounded::integer(value));
}

auto to_radix_sort_key(auto * ptr) {
	return reinterpret_cast<std::uintptr_t>(ptr);
}

} // namespace containers