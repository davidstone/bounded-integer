// Copyright David Stone 2017.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <bounded/detail/int128.hpp>

#include <cstdint>
#include <limits>
#include <type_traits>

namespace bounded {
namespace detail {

#if defined BOUNDED_DETAIL_HAS_128_BIT

template<typename T>
struct make_unsigned_c {
	using type = std::make_unsigned_t<T>;
};

template<>
struct make_unsigned_c<uint128_t> {
	using type = uint128_t;
};

template<>
struct make_unsigned_c<int128_t> {
	using type = uint128_t;
};

template<typename T>
using make_unsigned = typename make_unsigned_c<T>::type;

#else

template<typename T>
using make_unsigned = std::make_unsigned_t<T>;

#endif

template<typename T>
using promoted_unsigned = std::conditional_t<
	sizeof(T) <= sizeof(unsigned),
	unsigned,
	make_unsigned<std::decay_t<T>>
>;

#if defined BOUNDED_DETAIL_HAS_128_BIT
using max_signed_t = int128_t;
using max_unsigned_t = uint128_t;
#else
using max_signed_t = std::intmax_t;
using max_unsigned_t = std::uintmax_t;
#endif

template<typename T>
concept builtin_arithmetic = (std::is_arithmetic_v<T> and !std::is_same_v<T, bool>) or std::is_same_v<T, detail::max_signed_t> or std::is_same_v<T, detail::max_unsigned_t>;

template<typename T>
concept builtin_integer = builtin_arithmetic<T> and !std::is_floating_point_v<T>;

template<typename T>
concept signed_builtin = builtin_integer<T> and (std::is_signed_v<T> or std::is_same_v<T, detail::max_signed_t>);

template<typename T>
concept unsigned_builtin = builtin_integer<T> and (std::is_unsigned_v<T> or std::is_same_v<T, detail::max_unsigned_t>);

constexpr inline auto as_unsigned = [](auto const value) {
	return static_cast<promoted_unsigned<decltype(value)>>(value);
};

} // namespace detail
} // namespace bounded
