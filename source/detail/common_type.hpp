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

#ifndef RANGED_INTEGER_COMMON_TYPE_HPP_
#define RANGED_INTEGER_COMMON_TYPE_HPP_

#include "common_policy.hpp"
#include "enable_if.hpp"
#include "forward_declaration.hpp"
#include "is_ranged_integer.hpp"
#include "policy.hpp"
#include <type_traits>

namespace detail {
template<typename integer, typename... integers>
class all_are_ranged_or_builtin_integer {
public:
	static constexpr bool value = all_are_ranged_or_builtin_integer<integer>::value and all_are_ranged_or_builtin_integer<integers...>::value;
};

template<typename T>
class all_are_ranged_or_builtin_integer<T> {
public:
	static constexpr bool value = std::is_integral<T>::value or is_ranged_integer<T>();
};

template<typename T, typename... Ts>
class all_are_integral {
public:
	static constexpr bool value = all_are_integral<T>::value and all_are_integral<Ts...>::value;
};
template<typename T>
class all_are_integral<T> {
public:
	static constexpr bool value = std::is_integral<T>::value;
};

}	// namespace detail

namespace std {

// I do not have to specialize the single-argument version, as it just returns
// the type passed in, which will always work.

template<
	intmax_t lhs_min, intmax_t lhs_max, template<intmax_t, intmax_t> class lhs_policy,
	intmax_t rhs_min, intmax_t rhs_max, template<intmax_t, intmax_t> class rhs_policy
>
class common_type<ranged_integer<lhs_min, lhs_max, lhs_policy>, ranged_integer<rhs_min, rhs_max, rhs_policy>> {
private:
	static constexpr auto minimum = (lhs_min < rhs_min) ? lhs_min : rhs_min;
	static constexpr auto maximum = (lhs_max > rhs_max) ? lhs_max : rhs_max;
public:
	using type = typename common_policy<lhs_policy, rhs_policy>::template type<minimum, maximum>;
};

// Common type of a ranged_integer and a built-in
// Cannot seem to get this to work with enable_if
template<intmax_t minimum, intmax_t maximum, template<intmax_t, intmax_t> class overflow_policy, typename integer>
class common_type<ranged_integer<minimum, maximum, overflow_policy>, integer> {
private:
	using type1 = ranged_integer<minimum, maximum, overflow_policy>;
	using type2 = ranged_integer<static_cast<intmax_t>(std::numeric_limits<integer>::min()), static_cast<intmax_t>(std::numeric_limits<integer>::max()), overflow_policy>;
public:
	using type = typename common_type<type1, type2>::type;
};

// These specializations are needed to get proper behavior out of common_type.
// If I just get the common_type of a variadic list of types as passed in by the
// user, they could do something like
// common_type<int, unsigned, checked_integer<0, UINT_MAX + 1>>::type
// which would first convert int and unsigned to their common type (which is
// unsigned), then get the common type of that and the ranged_integer. However,
// the int type is outside of the range of the result, so the common type cannot
// actually safely store any value in the range. The only safe way is to make
// sure that I always get the common_type of a ranged_integer and another type,
// regardless of the order passed in by the user. The common type of int and
// checked_integer<0, UINT_MAX + 1> is checked_integer<INT_MIN, UINT_MAX + 1>,
// and the common_type of that and unsigned gives
// checked_integer<INT_MIN, UINT_MAX + 1>.
//
// Unfortunately, it looks like I have to specify each an arbitrary number of
// integral arguments prior to the ranged_integer, so I have to just specialize
// enough to make it unlikely to have a bunch of built-in integer types at the
// start followed by a ranged_integer.

template<
	typename integer1,
	intmax_t minimum, intmax_t maximum, template<intmax_t, intmax_t> class overflow_policy
>
class common_type<integer1, ranged_integer<minimum, maximum, overflow_policy>> {
public:
	using type = typename common_type<ranged_integer<minimum, maximum, overflow_policy>, integer1>::type;
};

// I only have to move the ranged_integer to the front once. After that, the
// default definition works.
template<
	typename integer1,
	typename integer2,
	intmax_t minimum, intmax_t maximum, template<intmax_t, intmax_t> class overflow_policy,
	typename... integers
>
class common_type<integer1, integer2, ranged_integer<minimum, maximum, overflow_policy>, integers...> {
public:
	using type = typename common_type<
		typename common_type<
			ranged_integer<minimum, maximum, overflow_policy>,
			integer1
		>::type,
		integer2,
		integers...
	>::type;
};

template<
	typename integer1,
	typename integer2,
	typename integer3,
	intmax_t minimum, intmax_t maximum, template<intmax_t, intmax_t> class overflow_policy,
	typename... integers
>
class common_type<integer1, integer2, integer3, ranged_integer<minimum, maximum, overflow_policy>, integers...> {
public:
	using type = typename common_type<
		typename common_type<
			ranged_integer<minimum, maximum, overflow_policy>,
			integer1
		>::type,
		integer2,
		integer3,
		integers...
	>::type;
};

template<
	typename integer1,
	typename integer2,
	typename integer3,
	typename integer4,
	intmax_t minimum, intmax_t maximum, template<intmax_t, intmax_t> class overflow_policy,
	typename... integers
>
class common_type<integer1, integer2, integer3, integer4, ranged_integer<minimum, maximum, overflow_policy>, integers...> {
public:
	using type = typename common_type<
		typename common_type<
			ranged_integer<minimum, maximum, overflow_policy>,
			integer1
		>::type,
		integer2,
		integer3,
		integer4,
		integers...
	>::type;
};

template<
	typename integer1,
	typename integer2,
	typename integer3,
	typename integer4,
	typename integer5,
	intmax_t minimum, intmax_t maximum, template<intmax_t, intmax_t> class overflow_policy,
	typename... integers
>
class common_type<integer1, integer2, integer3, integer4, integer5, ranged_integer<minimum, maximum, overflow_policy>, integers...> {
public:
	using type = typename common_type<
		typename common_type<
			ranged_integer<minimum, maximum, overflow_policy>,
			integer1
		>::type,
		integer2,
		integer3,
		integer4,
		integer5,
		integers...
	>::type;
};

}	// namespace std

template<typename... Ts>
using common_type_t = typename std::common_type<Ts...>::type;

#endif	// RANGED_INTEGER_COMMON_TYPE_HPP_
