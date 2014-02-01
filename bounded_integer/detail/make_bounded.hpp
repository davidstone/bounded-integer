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

template<typename integer>
class equivalent_overflow_policy_c {
public:
	using type = null_policy;
};
template<intmax_t minimum, intmax_t maximum, typename overflow_policy, bounds bound>
class equivalent_overflow_policy_c<bounded_integer<minimum, maximum, overflow_policy, bound>> {
public:
	using type = overflow_policy;
};

template<typename integer>
using equivalent_overflow_policy = typename equivalent_overflow_policy_c<integer>::type;

template<typename integer>
class equivalent_dynamic_bounds {
public:
	static constexpr bounds value = bounds::static_min_max;
};
template<intmax_t minimum, intmax_t maximum, typename overflow_policy, bounds bound>
class equivalent_dynamic_bounds<bounded_integer<minimum, maximum, overflow_policy, bound>> {
public:
	static constexpr bounds value = bound;
};

}	// namespace detail

template<typename integer, typename overflow_policy = detail::equivalent_overflow_policy<integer>, bounds bound = detail::equivalent_dynamic_bounds<integer>::value>
using equivalent_type = bounded_integer<
	static_cast<intmax_t>(std::numeric_limits<integer>::min()),
	static_cast<intmax_t>(std::numeric_limits<integer>::max()),
	overflow_policy,
	bound
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
// 2) The second parameter is similar to the first, but for dynamic bounds
// checking instead of overflow policy deduction.
//
// 3) The third parameter ("integer") has to be defaulted because we defaulted
// the first. We don't want "integer" to be the first parameter because that
// would prevent type deduction from the function argument. Therefore, we
// default it to something (it doesn't matter what), but the type will always be
// deduced as whatever we pass as the argument type.

template<
	typename overflow_policy = void,
	bounds bound = bounds::detail_any,
	typename integer = void,
	typename result_t = equivalent_type<
		integer,
		typename std::conditional<
			std::is_void<overflow_policy>::value,
			detail::equivalent_overflow_policy<integer>,
			overflow_policy
		>::type,
		(bound == bounds::detail_any ? detail::equivalent_dynamic_bounds<integer>::value : bound)
	>
>
constexpr result_t make_bounded(integer const value) noexcept {
	static_assert(std::numeric_limits<integer>::is_integer, "Must be an integer type.");
	return result_t(value, non_check);
}

template<intmax_t value, typename overflow_policy = null_policy, bounds bound = bounds::static_min_max>
constexpr bounded_integer<value, value, overflow_policy, bound> make_bounded() noexcept {
	return bounded_integer<value, value, overflow_policy, bound>(value, non_check);
}

}	// namespace bounded_integer
#endif	// BOUNDED_INTEGER_MAKE_BOUNDED_HPP_
