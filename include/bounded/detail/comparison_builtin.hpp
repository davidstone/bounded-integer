// Copyright David Stone 2015.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <bounded/detail/comparison.hpp>

#include <bounded/detail/basic_numeric_limits.hpp>
#include <bounded/detail/common_type.hpp>
#include <bounded/detail/forward_declaration.hpp>
#include <bounded/detail/make.hpp>
#include <bounded/detail/requires.hpp>

#include <cstdint>
#include <type_traits>

namespace bounded {

// Equality with built-ins

template<
	intmax_t minimum, intmax_t maximum, typename overflow_policy, storage_type storage, bool poisoned,
	typename T,
	BOUNDED_REQUIRES(basic_numeric_limits<T>::is_integer)
>
constexpr auto operator==(integer<minimum, maximum, overflow_policy, storage, poisoned> const lhs, T const rhs) noexcept {
	return lhs == make(rhs);
}

template<intmax_t minimum, intmax_t maximum, typename overflow_policy, storage_type storage, bool poisoned>
constexpr auto operator==(integer<minimum, maximum, overflow_policy, storage, poisoned> const lhs, uintmax_t const rhs) noexcept {
	return (lhs < 0) ? false : static_cast<uintmax_t>(lhs) == rhs;
}

template<
	intmax_t minimum, intmax_t maximum, typename overflow_policy, storage_type storage, bool poisoned,
	typename T,
	BOUNDED_REQUIRES(basic_numeric_limits<T>::is_integer)
>
constexpr auto operator==(T const lhs, integer<minimum, maximum, overflow_policy, storage, poisoned> const rhs) noexcept {
	return rhs == lhs;
}


// Relational operators with built-ins

template<
	intmax_t minimum, intmax_t maximum, typename overflow_policy, storage_type storage, bool poisoned,
	typename T,
	BOUNDED_REQUIRES(basic_numeric_limits<T>::is_integer)
>
constexpr auto operator<(integer<minimum, maximum, overflow_policy, storage, poisoned> const lhs, T const rhs) noexcept {
	return lhs < make(rhs);
}
template<intmax_t minimum, intmax_t maximum, typename overflow_policy, storage_type storage, bool poisoned>
constexpr auto operator<(integer<minimum, maximum, overflow_policy, storage, poisoned> const lhs, uintmax_t const rhs) noexcept {
	return (lhs < 0) ? true : static_cast<uintmax_t>(lhs) < rhs;
}

template<
	intmax_t minimum, intmax_t maximum, typename overflow_policy, storage_type storage, bool poisoned,
	typename T,
	BOUNDED_REQUIRES(basic_numeric_limits<T>::is_integer)
>
constexpr auto operator<(T const lhs, integer<minimum, maximum, overflow_policy, storage, poisoned> const rhs) noexcept {
	return make(lhs) < rhs;
}
template<intmax_t minimum, intmax_t maximum, typename overflow_policy, storage_type storage, bool poisoned>
constexpr auto operator<(uintmax_t const lhs, integer<minimum, maximum, overflow_policy, storage, poisoned> const rhs) noexcept {
	return (rhs < 0) ? false : lhs < static_cast<uintmax_t>(rhs);
}

}	// namespace bounded
