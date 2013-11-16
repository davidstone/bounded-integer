// Factory function that turns a built-in into a ranged_integer
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

#ifndef RANGED_INTEGER_MAKE_RANGED_HPP_
#define RANGED_INTEGER_MAKE_RANGED_HPP_

#include "class.hpp"
#include "enable_if.hpp"
#include "is_ranged_integer.hpp"
#include "policy.hpp"
#include <limits>

// This somewhat strange looking set of default arguments allows the following:
//
// 1) The first parameter is defaulted for the fairly common case of people
// needing to convert their built-in integer type to a ranged_integer. The
// ranged_integer analog to built-in integers is a ranged_integer with a
// null_policy.
//
// 2) The second parameter ("integer") has to be defaulted because we defaulted
// the first. We don't want "integer" to be the first parameter because that
// would prevent type deduction from the function argument. Therefore, we
// default it to something (it doesn't matter what), but the type will always be
// deduced as whatever we pass as the argument type.
//
// 3) Finally, we use the defaulted template argument version of enable_if,
// rather than using enable_if_t as seen elsewhere in this code because we have
// to put a defaulted template parameter at the end of other defaulted
// parameters.
template<template<intmax_t, intmax_t> class overflow_policy = null_policy, typename integer = void, typename = enable_if_t<std::is_integral<integer>::value or is_ranged_integer<integer>::value>>
constexpr ranged_integer<static_cast<intmax_t>(std::numeric_limits<integer>::min()), static_cast<intmax_t>(std::numeric_limits<integer>::max()), overflow_policy> make_ranged(integer const value) noexcept {
	return ranged_integer<static_cast<intmax_t>(std::numeric_limits<integer>::min()), static_cast<intmax_t>(std::numeric_limits<integer>::max()), overflow_policy>(value, non_check);
}

template<intmax_t value, template<intmax_t, intmax_t> class overflow_policy = null_policy>
constexpr ranged_integer<value, value, overflow_policy> make_ranged() noexcept {
	return ranged_integer<value, value, overflow_policy>(value, non_check);
}

#endif	// RANGED_INTEGER_MAKE_RANGED_HPP_
