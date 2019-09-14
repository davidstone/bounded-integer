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

template<typename T>
struct promoted_unsigned_c {
	using type = std::conditional_t<sizeof(T) <= sizeof(unsigned), unsigned, std::make_unsigned_t<T>>;
};

template<typename T>
using promoted_unsigned = typename promoted_unsigned_c<std::decay_t<T>>::type;

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

}	// namespace detail

// These are not in the detail namespace to work around
// https://github.com/saarraz/clang-concepts-monorepo/issues/18

template<typename T>
concept detail_builtin_integer = (std::is_integral_v<T> and !std::is_same_v<T, bool>) or std::is_same_v<T, detail::max_signed_t> or std::is_same_v<T, detail::max_unsigned_t>;

// Work around 
template<typename T>
concept detail_builtin_arithmetic = (std::is_arithmetic_v<T> and !std::is_same_v<T, bool>) or std::is_same_v<T, detail::max_signed_t> or std::is_same_v<T, detail::max_unsigned_t>;

template<typename T>
concept detail_signed_builtin = std::is_signed_v<T> or std::is_same_v<T, detail::max_signed_t>;
template<typename T>
concept detail_unsigned_builtin = std::is_unsigned_v<T> or std::is_same_v<T, detail::max_unsigned_t>;

}	// namespace bounded
