// Copyright David Stone 2015.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOUNDED_CONDITIONAL_HPP
#define BOUNDED_CONDITIONAL_HPP

import bounded.common_type_and_value_category;

#define BOUNDED_CONDITIONAL(condition, lhs, rhs) \
	((condition) ? \
		static_cast<bounded::common_type_and_value_category<decltype(lhs), decltype(rhs)>>(lhs) : \
		static_cast<bounded::common_type_and_value_category<decltype(lhs), decltype(rhs)>>(rhs))

#endif // BOUNDED_CONDITIONAL_HPP