// Factory function that turns a built-in into a bounded_integer
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

#ifndef BOUNDED_INTEGER_MAKE_BOUNDED_HPP_
#define BOUNDED_INTEGER_MAKE_BOUNDED_HPP_

#include "class.hpp"
#include "is_bounded_integer.hpp"
#include "numeric_limits.hpp"
#include "policy/null_policy.hpp"
#include <cstdint>
#include <type_traits>

namespace bounded_integer {
namespace detail {

template<typename T>
class equivalent_overflow_policy_c {
public:
	using type = null_policy;
};
template<intmax_t minimum, intmax_t maximum, typename overflow_policy>
class equivalent_overflow_policy_c<integer<minimum, maximum, overflow_policy>> {
public:
	using type = overflow_policy;
};

template<typename T>
using equivalent_overflow_policy = typename equivalent_overflow_policy_c<T>::type;

}	// namespace detail

template<typename T, typename overflow_policy = detail::equivalent_overflow_policy<T>>
using equivalent_type = integer<
	static_cast<intmax_t>(std::numeric_limits<T>::min()),
	static_cast<intmax_t>(std::numeric_limits<T>::max()),
	overflow_policy
>;

// This somewhat strange looking set of default arguments allows the following:
//
// 1) The first parameter is defaulted to void. The common case for this
// function is to convert a built-in integer type to a bounded_integer. The
// bounded_integer analog to built-in integers is a bounded_integer with a
// null_policy. However, if someone passes in something that is already a
// bounded_integer, this shouldn't change the policy. Therefore, we default to
// something that is not a valid policy (void) and choose correct default
// behavior based on that. The policy must be the first parameter to work
// properly with automatic deduction of the type of `integer`, which means that
// the type of integer is not known when we are trying to determine the default
// policy.
//
// 2) The second parameter ("integer") has to be defaulted because we defaulted
// the first. We don't want "integer" to be the first parameter because that
// would prevent type deduction from the function argument. Therefore, we
// default it to something (it doesn't matter what), but the type will always be
// deduced as whatever we pass as the argument type.

template<typename overflow_policy = void, typename T = void>
constexpr equivalent_type<T, typename std::conditional<std::is_void<overflow_policy>::value,
	detail::equivalent_overflow_policy<T>,
	overflow_policy
>::type> make(T const value) noexcept {
	static_assert(std::numeric_limits<T>::is_integer, "Must be an integer type.");
	return {value, non_check};
}

template<intmax_t value, typename overflow_policy = null_policy>
constexpr integer<value, value, overflow_policy> make() noexcept {
	return {value, non_check};
}

}	// namespace bounded_integer
#endif	// BOUNDED_INTEGER_MAKE_BOUNDED_HPP_
