// Copyright David Stone 2015.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include "common_type_and_value_category.hpp"

#define BOUNDED_CONDITIONAL(condition, lhs, rhs) \
	((condition) ? \
		static_cast<bounded::detail::common_type_and_value_category_t<decltype(lhs), decltype(rhs)>>(lhs) : \
		static_cast<bounded::detail::common_type_and_value_category_t<decltype(lhs), decltype(rhs)>>(rhs))

