// Copyright David Stone 2017.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <bounded/detail/basic_numeric_limits.hpp>
#include <bounded/detail/class.hpp>
#include <bounded/detail/max_builtin.hpp>
#include <bounded/detail/modulo_cast.hpp>

#include <limits>

namespace bounded {
namespace detail {

template<auto value>
constexpr auto safer_negation() {
	constexpr auto negated = -static_cast<max_unsigned_t>(value);
	if constexpr (value <= 0) {
		return negated;
	} else {
		constexpr auto max_positive_input = -static_cast<max_unsigned_t>(basic_numeric_limits<max_signed_t>::min());
		static_assert(value <= max_positive_input, "Negation would overflow.");
		return static_cast<max_signed_t>(negated);
	}
}

}	// namespace detail

template<auto minimum, auto maximum, typename overflow_policy>
constexpr auto operator-(integer<minimum, maximum, overflow_policy> const value) {
	using result_type = integer<
		detail::normalize<detail::safer_negation<maximum>()>,
		detail::normalize<detail::safer_negation<minimum>()>,
		overflow_policy
	>;
	using underlying_type = typename result_type::underlying_type;
	using promoted_unsigned_type = detail::promoted_unsigned<underlying_type>;
	return result_type(
		detail::modulo_cast<underlying_type>(-static_cast<promoted_unsigned_type>(value)),
		non_check
	);
}

}	// namespace bounded

