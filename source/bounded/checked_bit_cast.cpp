// Copyright David Stone 2024.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

export module bounded.checked_bit_cast;

import bounded.bounded_integer;
import bounded.check_in_range;

import std_module;

namespace bounded {

export template<bounded_integer T, typename Source>
constexpr auto checked_bit_cast(Source const source) -> T {
	return bounded::check_in_range<T>(std::bit_cast<typename T::underlying_type>(source));
}

} // namespace bounded
