// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/algorithms/compare.hpp>
#include <containers/append.hpp>
#include <containers/assign.hpp>
#include <containers/clear.hpp>
#include <containers/erase.hpp>
#include <containers/insert.hpp>
#include <containers/non_modifying_common_container_functions.hpp>
#include <containers/push_back.hpp>
#include <containers/resize.hpp>

namespace containers {
namespace detail {
namespace common {

#define CONTAINERS_COMMON_USING_DECLARATIONS \
	CONTAINERS_NON_MODIFYING_COMMON_USING_DECLARATIONS \
	using ::containers::detail::common::push_back; \
	using ::containers::detail::common::append; \
	using ::containers::detail::common::insert; \
	using ::containers::detail::common::erase; \
	using ::containers::detail::common::erase_if; \
	using ::containers::detail::common::assign; \
	using ::containers::detail::common::clear; \
	using ::containers::detail::common::resize;

}	// namespace common

CONTAINERS_COMMON_USING_DECLARATIONS

}	// namespace detail

CONTAINERS_COMMON_USING_DECLARATIONS

}	// namespace containers
