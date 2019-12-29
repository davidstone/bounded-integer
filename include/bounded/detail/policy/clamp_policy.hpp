// Copyright David Stone 2017.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <bounded/detail/policy/null_policy.hpp>
#include <bounded/detail/class.hpp>
#include <bounded/detail/is_bounded_integer.hpp>
#include <bounded/detail/minmax.hpp>

namespace bounded {

struct clamp_policy {
	static constexpr auto assignment(bounded_integer auto const & value, bounded_integer auto const & minimum, bounded_integer auto const & maximum) {
		constexpr auto policy = bounded::null_policy();
		return min(
			max(
				integer(value, policy),
				integer(minimum, policy)
			),
			integer(maximum, policy)
		);
	}

	using overflow_policy_tag = void;
	static constexpr bool is_modulo = false;
	static constexpr bool overflow_is_error = false;
};

template<auto minimum, auto maximum>
using clamped_integer = integer<minimum, maximum, clamp_policy>;

}	// namespace bounded
