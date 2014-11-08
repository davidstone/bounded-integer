// ==    !=    <    <=    >    >= with built-in types as one side
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

#ifndef BOUNDED_INTEGER_OPERATORS_COMPARISON_BUILTIN_HPP_
#define BOUNDED_INTEGER_OPERATORS_COMPARISON_BUILTIN_HPP_

#include "comparison.hpp"

#include "../common_type.hpp"
#include "../enable_if.hpp"
#include "../forward_declaration.hpp"
#include "../make.hpp"
#include "../numeric_limits.hpp"

#include <cstdint>
#include <type_traits>

namespace bounded {

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
#endif	// BOUNDED_INTEGER_OPERATORS_COMPARISON_BUILTIN_HPP_
