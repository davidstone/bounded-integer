// Copyright David Stone 2017.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <cstdint>
#include <limits>
#include <type_traits>

namespace bounded {
namespace detail {

#if defined __GNUC__ or defined __Clang__
	#define BOUNDED_DETAIL_HAS_128_BIT 1
	using int128_t = __int128_t;
	using uint128_t = __uint128_t;
#endif

#if defined BOUNDED_DETAIL_HAS_128_BIT
using max_signed_t = int128_t;
using max_unsigned_t = uint128_t;

static_assert(sizeof(max_signed_t) > sizeof(std::intmax_t));
static_assert(sizeof(max_unsigned_t) > sizeof(std::uintmax_t));

#else
using max_signed_t = std::intmax_t;
using max_unsigned_t = std::uintmax_t;
#endif

template<typename T>
constexpr auto is_builtin_integer = std::is_integral<T>{} or std::is_same<T, max_signed_t>{} or std::is_same<T, max_unsigned_t>{};

template<typename T>
constexpr auto is_builtin_arithmetic = std::is_arithmetic<T>{} or std::is_same<T, max_signed_t>{} or std::is_same<T, max_unsigned_t>{};

template<typename T>
constexpr auto is_signed_builtin = std::is_signed<T>{} or std::is_same<T, max_signed_t>{};
template<typename T>
constexpr auto is_unsigned_builtin = std::is_unsigned<T>{} or std::is_same<T, max_unsigned_t>{};

}	// namespace detail
}	// namespace bounded
