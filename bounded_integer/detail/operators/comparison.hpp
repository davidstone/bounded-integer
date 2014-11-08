// ==    !=    <    <=    >    >=
// Copyright (C) 2014 David Stone
//
// This program is free software: you can redistribute it and / or modify
// it under the terms of the GNU Affero General Public License as
// published by the Free Software Foundation, either version 3 of the
// License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU Affero General Public License for more details.
//
// You should have received a copy of the GNU Affero General Public License
// along with this program. If not, see <http://www.gnu.org/licenses/>.

#ifndef BOUNDED_INTEGER_COMPARISON_OPERATORS_HPP_
#define BOUNDED_INTEGER_COMPARISON_OPERATORS_HPP_

#include "../common_type.hpp"
#include "../enable_if.hpp"
#include "../forward_declaration.hpp"
#include "../make.hpp"
#include "../numeric_limits.hpp"
#include "../policy/null_policy.hpp"

#include <cstdint>
#include <type_traits>

namespace bounded {

// In my original implementation, I wrote these as comparisons between a
// bounded::integer and its underlying type. However, this actually makes
// relational operators much less useful than they otherwise could be. For
// instance, if we have bounded::integer<-100, 100>, we might want to compare
// that with an int64_t. If we have to first convert the value to the underlying
// type (likely int8_t), then we are likely to invoke undefined behavior.
// Instead, the comparison function should accept a much wider array of types
// and convert up to the common type.

namespace detail {

// This preferentially uses storage_type::fast, but does no conversions if they
// are the same. If new storage types are added, this will preferentially use
// storage_type::least over whatever that new type is.
constexpr storage_type comparison_storage_type(storage_type lhs_storage, storage_type rhs_storage) noexcept {
	return (lhs_storage <= rhs_storage) ? lhs_storage : rhs_storage;
}

// This uses the null_policy because policies do not matter for comparisons.
// This allows the user to compare integers with unrelated policies.
template<intmax_t lhs_min, intmax_t lhs_max, storage_type lhs_storage, intmax_t rhs_min, intmax_t rhs_max, storage_type rhs_storage>
using comparison_type = typename std::common_type_t<
	integer<lhs_min, lhs_max, null_policy, comparison_storage_type(lhs_storage, rhs_storage)>,
	integer<rhs_min, rhs_max, null_policy, comparison_storage_type(lhs_storage, rhs_storage)>
>::underlying_type const;

}	// namespace detail

// Equality
template<
	intmax_t lhs_min, intmax_t lhs_max, typename lhs_overflow, storage_type lhs_storage,
	intmax_t rhs_min, intmax_t rhs_max, typename rhs_overflow, storage_type rhs_storage
>
constexpr auto operator==(integer<lhs_min, lhs_max, lhs_overflow, lhs_storage> const lhs, integer<rhs_min, rhs_max, rhs_overflow, rhs_storage> const rhs) noexcept {
	using common_t = detail::comparison_type<lhs_min, lhs_max, lhs_storage, rhs_min, rhs_max, rhs_storage>;
	return static_cast<common_t>(lhs) == static_cast<common_t>(rhs);
}

template<
	intmax_t lhs_min, intmax_t lhs_max, typename lhs_overflow, storage_type lhs_storage,
	intmax_t rhs_min, intmax_t rhs_max, typename rhs_overflow, storage_type rhs_storage
>
constexpr auto operator!=(integer<lhs_min, lhs_max, lhs_overflow, lhs_storage> const lhs, integer<rhs_min, rhs_max, rhs_overflow, rhs_storage> const rhs) noexcept {
	return !(lhs == rhs);
}



// Equality with built-ins

template<
	intmax_t minimum, intmax_t maximum, typename overflow_policy, storage_type storage,
	typename T,
	enable_if_t<detail::basic_numeric_limits<T>::is_integer> = clang_dummy
>
constexpr auto operator==(integer<minimum, maximum, overflow_policy, storage> const lhs, T const rhs) noexcept {
	return lhs == make(rhs);
}

template<intmax_t minimum, intmax_t maximum, typename overflow_policy, storage_type storage>
constexpr auto operator==(integer<minimum, maximum, overflow_policy, storage> const lhs, uintmax_t const rhs) noexcept {
	return (lhs < 0) ? false : static_cast<uintmax_t>(lhs) == rhs;
}

template<
	intmax_t minimum, intmax_t maximum, typename overflow_policy, storage_type storage,
	typename T,
	enable_if_t<detail::basic_numeric_limits<T>::is_integer> = clang_dummy
>
constexpr auto operator==(T const lhs, integer<minimum, maximum, overflow_policy, storage> const rhs) noexcept {
	return rhs == lhs;
}

template<
	intmax_t minimum, intmax_t maximum, typename overflow_policy, storage_type storage,
	typename T,
	enable_if_t<detail::basic_numeric_limits<T>::is_integer> = clang_dummy
>
constexpr auto operator!=(integer<minimum, maximum, overflow_policy, storage> const lhs, T const rhs) noexcept {
	return !(lhs == rhs);
}

template<
	intmax_t minimum, intmax_t maximum, typename overflow_policy, storage_type storage,
	typename T,
	enable_if_t<detail::basic_numeric_limits<T>::is_integer> = clang_dummy
>
constexpr auto operator!=(T const lhs, integer<minimum, maximum, overflow_policy, storage> const rhs) noexcept {
	return !(rhs == lhs);
}



// Relational operators

template<
	intmax_t lhs_min, intmax_t lhs_max, typename lhs_overflow, storage_type lhs_storage,
	intmax_t rhs_min, intmax_t rhs_max, typename rhs_overflow, storage_type rhs_storage
>
constexpr auto operator<(integer<lhs_min, lhs_max, lhs_overflow, lhs_storage> const lhs, integer<rhs_min, rhs_max, rhs_overflow, rhs_storage> const rhs) noexcept {
	using common_t = detail::comparison_type<lhs_min, lhs_max, lhs_storage, rhs_min, rhs_max, rhs_storage>;
	return static_cast<common_t>(lhs) < static_cast<common_t>(rhs);
}

template<
	intmax_t lhs_min, intmax_t lhs_max, typename lhs_overflow, storage_type lhs_storage,
	intmax_t rhs_min, intmax_t rhs_max, typename rhs_overflow, storage_type rhs_storage
>
constexpr auto operator>(integer<lhs_min, lhs_max, lhs_overflow, lhs_storage> const lhs, integer<rhs_min, rhs_max, rhs_overflow, rhs_storage> const rhs) noexcept {
	return rhs < lhs;
}

template<
	intmax_t lhs_min, intmax_t lhs_max, typename lhs_overflow, storage_type lhs_storage,
	intmax_t rhs_min, intmax_t rhs_max, typename rhs_overflow, storage_type rhs_storage
>
constexpr auto operator<=(integer<lhs_min, lhs_max, lhs_overflow, lhs_storage> const lhs, integer<rhs_min, rhs_max, rhs_overflow, rhs_storage> const rhs) noexcept {
	return !(rhs < lhs);
}

template<
	intmax_t lhs_min, intmax_t lhs_max, typename lhs_overflow, storage_type lhs_storage,
	intmax_t rhs_min, intmax_t rhs_max, typename rhs_overflow, storage_type rhs_storage
>
constexpr auto operator>=(integer<lhs_min, lhs_max, lhs_overflow, lhs_storage> const lhs, integer<rhs_min, rhs_max, rhs_overflow, rhs_storage> const rhs) noexcept {
	return !(lhs < rhs);
}



// Relational operators with built-ins

template<
	intmax_t minimum, intmax_t maximum, typename overflow_policy, storage_type storage,
	typename T,
	enable_if_t<detail::basic_numeric_limits<T>::is_integer> = clang_dummy
>
constexpr auto operator<(integer<minimum, maximum, overflow_policy, storage> const lhs, T const rhs) noexcept {
	return lhs < make(rhs);
}
template<intmax_t minimum, intmax_t maximum, typename overflow_policy, storage_type storage>
constexpr auto operator<(integer<minimum, maximum, overflow_policy, storage> const lhs, uintmax_t const rhs) noexcept {
	return (lhs < 0) ? true : static_cast<uintmax_t>(lhs) < rhs;
}

template<
	intmax_t minimum, intmax_t maximum, typename overflow_policy, storage_type storage,
	typename T,
	enable_if_t<detail::basic_numeric_limits<T>::is_integer> = clang_dummy
>
constexpr auto operator<(T const lhs, integer<minimum, maximum, overflow_policy, storage> const rhs) noexcept {
	return make(lhs) < rhs;
}
template<intmax_t minimum, intmax_t maximum, typename overflow_policy, storage_type storage>
constexpr auto operator<(uintmax_t const lhs, integer<minimum, maximum, overflow_policy, storage> const rhs) noexcept {
	return (rhs < 0) ? false : lhs < static_cast<uintmax_t>(rhs);
}

template<
	intmax_t minimum, intmax_t maximum, typename overflow_policy, storage_type storage,
	typename T,
	enable_if_t<detail::basic_numeric_limits<T>::is_integer> = clang_dummy
>
constexpr auto operator>(integer<minimum, maximum, overflow_policy, storage> const lhs, T const rhs) noexcept {
	return rhs < lhs;
}

template<
	intmax_t minimum, intmax_t maximum, typename overflow_policy, storage_type storage,
	typename T,
	enable_if_t<detail::basic_numeric_limits<T>::is_integer> = clang_dummy
>
constexpr auto operator>(T const lhs, integer<minimum, maximum, overflow_policy, storage> const rhs) noexcept {
	return rhs < lhs;
}

template<
	intmax_t minimum, intmax_t maximum, typename overflow_policy, storage_type storage,
	typename T,
	enable_if_t<detail::basic_numeric_limits<T>::is_integer> = clang_dummy
>
constexpr auto operator<=(integer<minimum, maximum, overflow_policy, storage> const lhs, T const rhs) noexcept {
	return !(rhs < lhs);
}

template<
	intmax_t minimum, intmax_t maximum, typename overflow_policy, storage_type storage,
	typename T,
	enable_if_t<detail::basic_numeric_limits<T>::is_integer> = clang_dummy
>
constexpr auto operator<=(T const lhs, integer<minimum, maximum, overflow_policy, storage> const rhs) noexcept {
	return !(rhs < lhs);
}

template<
	intmax_t minimum, intmax_t maximum, typename overflow_policy, storage_type storage,
	typename T,
	enable_if_t<detail::basic_numeric_limits<T>::is_integer> = clang_dummy
>
constexpr auto operator>=(integer<minimum, maximum, overflow_policy, storage> const lhs, T const rhs) noexcept {
	return !(lhs < rhs);
}

template<
	intmax_t minimum, intmax_t maximum, typename overflow_policy, storage_type storage,
	typename T,
	enable_if_t<detail::basic_numeric_limits<T>::is_integer> = clang_dummy
>
constexpr auto operator>=(T const lhs, integer<minimum, maximum, overflow_policy, storage> const rhs) noexcept {
	return !(lhs < rhs);
}

}	// namespace bounded
#endif	// BOUNDED_INTEGER_COMPARISON_OPERATORS_HPP_
