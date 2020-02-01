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

template<typename T>
concept builtin_arithmetic = (std::is_arithmetic_v<T> and !std::is_same_v<T, bool>) or std::is_same_v<T, detail::max_signed_t> or std::is_same_v<T, detail::max_unsigned_t>;

template<typename T>
concept builtin_integer = builtin_arithmetic<T> and !std::is_floating_point_v<T>;

template<typename T>
concept signed_builtin = builtin_integer<T> and (std::is_signed_v<T> or std::is_same_v<T, detail::max_signed_t>);

template<typename T>
concept unsigned_builtin = builtin_integer<T> and (std::is_unsigned_v<T> or std::is_same_v<T, detail::max_unsigned_t>);

} // namespace detail
} // namespace bounded
