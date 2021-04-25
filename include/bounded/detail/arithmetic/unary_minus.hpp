// Copyright David Stone 2017.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <bounded/detail/class.hpp>
#include <bounded/detail/max_builtin.hpp>

#include <numeric_traits/min_max_value.hpp>

namespace bounded {
namespace detail {

template<auto value>
constexpr auto safer_negation() {
	constexpr auto negated = -static_cast<max_unsigned_t>(value);
	if constexpr (value <= 0) {
		return negated;
	} else {
		constexpr auto max_positive_input = -static_cast<max_unsigned_t>(numeric_traits::min_value<max_signed_t>);
		static_assert(value <= max_positive_input, "Negation would overflow.");
		return static_cast<max_signed_t>(negated);
	}
}

}	// namespace detail

template<auto minimum, auto maximum>
constexpr auto operator-(integer<minimum, maximum> const value) {
	using result_type = integer<
		normalize<detail::safer_negation<maximum>()>,
		normalize<detail::safer_negation<minimum>()>
	>;
	using underlying_type = typename result_type::underlying_type;
	using promoted_unsigned_type = detail::promoted_unsigned<underlying_type>;
	return result_type(
		static_cast<underlying_type>(-static_cast<promoted_unsigned_type>(value)),
		non_check
	);
}

}	// namespace bounded

