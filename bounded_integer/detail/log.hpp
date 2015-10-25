// Overloads for the standard library math functions.
// Copyright (C) 2015 David Stone
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

#pragma once

#include "basic_numeric_limits.hpp"
#include "class.hpp"
#include "is_bounded_integer.hpp"
#include "requires.hpp"

namespace bounded {
namespace detail {

template<typename Integer, typename Base, BOUNDED_REQUIRES(std::is_integral<Integer>::value)>
constexpr auto log(Integer value, Base const base) noexcept {
	auto sum = 0;
	while (value >= base) {
		value /= base;
		++sum;
	}
	return sum;
}

}	// namespace detail

template<
	intmax_t minimum, intmax_t maximum, typename overflow_policy, storage_type storage, bool poisoned,
	typename Base,
	BOUNDED_REQUIRES(is_bounded_integer<Base>)
>
constexpr auto log(integer<minimum, maximum, overflow_policy, storage, poisoned> const value, Base const base) noexcept {
	static_assert(base > constant<1>, "Negative bases not currently supported.");
	static_assert(value > constant<0>, "The log of a negative number or zero is undefined.");
	using result_type = integer<
		detail::log(minimum, basic_numeric_limits<Base>::max()),
		detail::log(maximum, basic_numeric_limits<Base>::min()),
		overflow_policy,
		storage,
		poisoned
	>;
	return result_type(detail::log(value.value(), base.value()), non_check);
}

}	// namespace bounded

namespace std {
// This overload is required for std::sort to work on gcc with iterators whose
// difference_type is a bounded::integer. It is not guaranteed to continue to be
// supported. It returns a built-in integer that is the log2 of the number
template<intmax_t minimum, intmax_t maximum, typename overflow_policy, bounded::storage_type storage, bool poisoned>
constexpr auto __lg(bounded::integer<minimum, maximum, overflow_policy, storage, poisoned> const value) noexcept {
	return __lg(value.value());
}

}	// namespace std
