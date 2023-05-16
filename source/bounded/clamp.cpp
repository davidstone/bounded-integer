// Copyright David Stone 2020.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

export module bounded.clamp;

import bounded.integer;
import bounded.is_bounded_integer;
import bounded.minmax;

namespace bounded {

export constexpr auto clamp(bounded_integer auto const & value, bounded_integer auto const & minimum, bounded_integer auto const & maximum) {
	return min(
		max(
			value,
			minimum
		),
		maximum
	);
}

} // namespace bounded

constexpr auto minimum = bounded::constant<27>;
constexpr auto maximum = bounded::constant<567>;
static_assert(
	clamp(bounded::constant<20>, minimum, maximum) == minimum,
	"Failure to properly clamp lesser positive values."
);
static_assert(
	clamp(bounded::constant<-25>, minimum, maximum) == minimum,
	"Failure to properly clamp negative values to a positive value."
);
static_assert(
	clamp(bounded::constant<1000>, minimum, maximum) == maximum,
	"Failure to properly clamp greater positive values."
);
static_assert(
	clamp(bounded::constant<2000>, minimum, maximum) == maximum,
	"Fail to clamp above range with a strictly greater type."
);
