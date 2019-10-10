// Copyright David Stone 2019.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <bounded/detail/forward_declaration.hpp>
#include <bounded/detail/int128.hpp>
#include <bounded/detail/max_builtin.hpp>
#include <bounded/detail/policy/null_policy.hpp>

#include <limits>
#include <type_traits>

namespace bounded {
namespace detail {

struct incomplete;

} // namespace detail

// You cannot `= delete` a variable template
template<typename>
extern detail::incomplete max_value;

template<typename>
extern detail::incomplete min_value;

template<typename T>
inline constexpr auto const & max_value<T const> = max_value<T>;

template<typename T>
inline constexpr auto const & max_value<T volatile> = max_value<T>;

template<typename T>
inline constexpr auto const & max_value<T const volatile> = max_value<T>;

template<typename T>
inline constexpr auto const & min_value<T const> = min_value<T>;

template<typename T>
inline constexpr auto const & min_value<T volatile> = min_value<T>;

template<typename T>
inline constexpr auto const & min_value<T const volatile> = min_value<T>;

template<detail_unsigned_builtin T>
inline constexpr auto max_value<T> = T(-1);

template<detail_unsigned_builtin T>
inline constexpr auto min_value<T> = T(0);

template<typename T> requires std::is_signed_v<T>
inline constexpr auto max_value<T> = std::numeric_limits<T>::max();

template<typename T> requires std::is_signed_v<T>
inline constexpr auto min_value<T> = std::numeric_limits<T>::min();

#if defined BOUNDED_DETAIL_HAS_128_BIT

template<>
inline constexpr auto max_value<detail::int128_t> = static_cast<detail::int128_t>(max_value<detail::uint128_t> / 2);

template<>
inline constexpr auto min_value<detail::int128_t> = -max_value<detail::int128_t> - 1;

#endif

template<auto minimum, auto maximum, typename overflow_policy>
inline constexpr auto max_value<integer<minimum, maximum, overflow_policy>> = integer<maximum, maximum, null_policy>();

template<auto minimum, auto maximum, typename overflow_policy>
inline constexpr auto min_value<integer<minimum, maximum, overflow_policy>> = integer<minimum, minimum, null_policy>();

template<typename T, T value>
inline constexpr auto max_value<std::integral_constant<T, value>> = value;

template<typename T, T value>
inline constexpr auto min_value<std::integral_constant<T, value>> = value;

}	// namespace bounded
