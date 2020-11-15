// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/non_modifying_common_container_functions.hpp>

namespace containers {
namespace detail {
namespace common {

#define CONTAINERS_COMMON_USING_DECLARATIONS \
	CONTAINERS_NON_MODIFYING_COMMON_USING_DECLARATIONS

}	// namespace common

CONTAINERS_COMMON_USING_DECLARATIONS

}	// namespace detail

CONTAINERS_COMMON_USING_DECLARATIONS

}	// namespace containers
