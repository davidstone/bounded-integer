// Type that is capable of holding the value in any of a list of integer types
// Copyright (C) 2013 David Stone
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

#ifndef BOUNDED_INTEGER_COMMON_TYPE_HPP_
#define BOUNDED_INTEGER_COMMON_TYPE_HPP_

#include "common_policy.hpp"
#include "forward_declaration.hpp"
#include "make_bounded.hpp"

#include <type_traits>

namespace bounded_integer {

template<typename T>
using decay_t = typename std::decay<T>::type;

// decay is temporary workaround
template<typename... Ts>
using common_type_t = typename std::common_type<decay_t<Ts>...>::type;

}	// namespace bounded_integer

namespace std {

// I do not have to specialize the single-argument version, as it just returns
// the type passed in, which will always work.

template<
	intmax_t lhs_min, intmax_t lhs_max, typename lhs_policy,
	intmax_t rhs_min, intmax_t rhs_max, typename rhs_policy
>
struct common_type<
	bounded_integer::bounded_integer<lhs_min, lhs_max, lhs_policy>,
	bounded_integer::bounded_integer<rhs_min, rhs_max, rhs_policy>
> {
private:
	static constexpr auto minimum = (lhs_min < rhs_min) ? lhs_min : rhs_min;
	static constexpr auto maximum = (lhs_max > rhs_max) ? lhs_max : rhs_max;
public:
	using type = bounded_integer::bounded_integer<minimum, maximum, bounded_integer::common_policy_t<lhs_policy, rhs_policy>>;
};

// Common type of a bounded_integer and a built-in
template<intmax_t minimum, intmax_t maximum, typename overflow_policy, typename integer>
struct common_type<bounded_integer::bounded_integer<minimum, maximum, overflow_policy>, integer> {
private:
	using type1 = bounded_integer::bounded_integer<minimum, maximum, overflow_policy>;
	using type2 = bounded_integer::equivalent_type<bounded_integer::decay_t<integer>>;
public:
	using type = bounded_integer::common_type_t<type1, type2>;
};

// If the user tries to do something like
// common_type<int, unsigned, checked_integer<0, UINT_MAX + 1>>::type
// this would first convert int and unsigned to their common type (which is
// unsigned), then get the common type of that and the bounded_integer. However,
// the int type is outside of the range of the result, so the common type cannot
// actually safely store any value in the range. The only safe way is to make
// sure that I always get the common_type of a bounded_integer and another type,
// regardless of the order passed in by the user. The common type of int and
// checked_integer<0, UINT_MAX + 1> is checked_integer<INT_MIN, UINT_MAX + 1>,
// and the common_type of that and unsigned gives
// checked_integer<INT_MIN, UINT_MAX + 1>.
//
// Unfortunately, it looks like I have to specify each an arbitrary number of
// integral arguments prior to the bounded_integer. There doesn't seem to be a
// general solution to the problem, so I am avoiding defining extended
// specializations for now.

template<typename integer1, intmax_t minimum, intmax_t maximum, typename overflow_policy>
struct common_type<integer1, bounded_integer::bounded_integer<minimum, maximum, overflow_policy>> {
public:
	using type = bounded_integer::common_type_t<bounded_integer::bounded_integer<minimum, maximum, overflow_policy>, integer1>;
};


// We use common_type heavily in the creation of deduced arrays, so we need to
// add in some tricks to limit the maximum instantiation depth:

template<intmax_t minimum, intmax_t maximum, typename overflow_policy, typename T1, typename T2, typename T3, typename T4, typename T5, typename... Ts>
struct common_type<bounded_integer::bounded_integer<minimum, maximum, overflow_policy>, T1, T2, T3, T4, T5, Ts...> {
private:
	using type0 = bounded_integer::bounded_integer<minimum, maximum, overflow_policy>;
public:
	using type = bounded_integer::common_type_t<bounded_integer::common_type_t<type0, T1, T2, T3, T4>, T5, Ts...>;
};

template<intmax_t minimum, intmax_t maximum, typename overflow_policy, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9, typename... Ts>
struct common_type<bounded_integer::bounded_integer<minimum, maximum, overflow_policy>, T1, T2, T3, T4, T5, T6, T7, T8, T9, Ts...> {
private:
	using type0 = bounded_integer::bounded_integer<minimum, maximum, overflow_policy>;
public:
	using type = bounded_integer::common_type_t<bounded_integer::common_type_t<type0, T1, T2, T3, T4, T5, T6, T7, T8>, T9, Ts...>;
};

}	// namespace std

#endif	// BOUNDED_INTEGER_COMMON_TYPE_HPP_
