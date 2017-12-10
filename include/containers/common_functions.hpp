// Copyright David Stone 2015.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/index_type.hpp>

#include <bounded/integer.hpp>

namespace containers {
namespace detail {

#define CONTAINERS_DETAIL_COMMON_USING_DECLARATIONS \
	BOUNDED_COMPARISON \
	BOUNDED_COMMON_ARITHMETIC

CONTAINERS_DETAIL_COMMON_USING_DECLARATIONS

}	// namespace detail

CONTAINERS_DETAIL_COMMON_USING_DECLARATIONS

}	// namespace containers
