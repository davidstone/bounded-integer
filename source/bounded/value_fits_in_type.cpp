// Copyright David Stone 2017.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

export module bounded.value_fits_in_type;

import bounded.builtin_min_max_value;
import bounded.comparison;
import bounded.safe_compare;

namespace bounded {

export template<typename T>
constexpr auto value_fits_in_type(auto const value) {
	return
		safe_compare(builtin_min_value<T>, value) <= 0 and
		safe_compare(value, builtin_max_value<T>) <= 0;
}

} // namespace bounded
