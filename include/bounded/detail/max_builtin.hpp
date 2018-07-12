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

template<typename T>
struct promoted_unsigned_c {
	using type = std::conditional_t<sizeof(T) <= sizeof(unsigned), unsigned, std::make_unsigned_t<T>>;
};

template<typename T>
using promoted_unsigned = typename promoted_unsigned_c<std::decay_t<T>>::type;

constexpr auto as_unsigned = [](auto const value) noexcept {
	return static_cast<promoted_unsigned<decltype(value)>>(value);
};

#if defined __GNUC__ and !defined __clang__
	#define BOUNDED_DETAIL_HAS_128_BIT 1
	using int128_t = __int128_t;
	using uint128_t = __uint128_t;
#endif

#if defined BOUNDED_DETAIL_HAS_128_BIT

template<>
struct promoted_unsigned_c<int128_t> {
	using type = uint128_t;
};
template<>
struct promoted_unsigned_c<uint128_t> {
	using type = uint128_t;
};


using max_signed_t = int128_t;
using max_unsigned_t = uint128_t;

static_assert(sizeof(max_signed_t) > sizeof(std::intmax_t));
static_assert(sizeof(max_unsigned_t) > sizeof(std::uintmax_t));

#else
using max_signed_t = std::intmax_t;
using max_unsigned_t = std::uintmax_t;
#endif

template<typename T>
constexpr auto is_builtin_integer = std::is_integral_v<T> or std::is_same_v<T, max_signed_t> or std::is_same_v<T, max_unsigned_t>;

template<typename T>
constexpr auto is_builtin_arithmetic = std::is_arithmetic_v<T> or std::is_same_v<T, max_signed_t> or std::is_same_v<T, max_unsigned_t>;

template<typename T>
constexpr auto is_signed_builtin = std::is_signed_v<T> or std::is_same_v<T, max_signed_t>;
template<typename T>
constexpr auto is_unsigned_builtin = std::is_unsigned_v<T> or std::is_same_v<T, max_unsigned_t>;

}	// namespace detail
}	// namespace bounded
