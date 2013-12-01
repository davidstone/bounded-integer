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

#include <limits>
#include <type_traits>

template<typename T>
using decay_t = typename std::decay<T>::type;

// decay is temporary workaround
template<typename... Ts>
using common_type_t = typename std::common_type<decay_t<Ts>...>::type;

namespace detail {
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
	intmax_t lhs_min, intmax_t lhs_max, typename lhs_policy,
	intmax_t rhs_min, intmax_t rhs_max, typename rhs_policy
>
class common_type<ranged_integer<lhs_min, lhs_max, lhs_policy>, ranged_integer<rhs_min, rhs_max, rhs_policy>> {
private:
	static constexpr auto minimum = (lhs_min < rhs_min) ? lhs_min : rhs_min;
	static constexpr auto maximum = (lhs_max > rhs_max) ? lhs_max : rhs_max;
public:
	using type = ranged_integer<minimum, maximum, common_policy_t<lhs_policy, rhs_policy>>;
};

// Common type of a ranged_integer and a built-in
// Cannot seem to get this to work with enable_if
template<intmax_t minimum, intmax_t maximum, typename overflow_policy, typename integer>
class common_type<ranged_integer<minimum, maximum, overflow_policy>, integer> {
private:
	using type1 = ranged_integer<minimum, maximum, overflow_policy>;
	using decayed_integer = decay_t<integer>;
	using type2 = ranged_integer<static_cast<intmax_t>(std::numeric_limits<decayed_integer>::min()), static_cast<intmax_t>(std::numeric_limits<decayed_integer>::max()), overflow_policy>;
public:
	using type = common_type_t<type1, type2>;
};

// If the user tries to do something like
// common_type<int, unsigned, checked_integer<0, UINT_MAX + 1>>::type
// this would first convert int and unsigned to their common type (which is
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
// integral arguments prior to the ranged_integer. There doesn't seem to be a
// general solution to the problem, so I am avoiding defining extended
// specializations for now.

template<
	typename integer1,
	intmax_t minimum, intmax_t maximum, typename overflow_policy
>
class common_type<integer1, ranged_integer<minimum, maximum, overflow_policy>> {
public:
	using type = common_type_t<ranged_integer<minimum, maximum, overflow_policy>, integer1>;
};

}	// namespace std

#endif	// RANGED_INTEGER_COMMON_TYPE_HPP_
