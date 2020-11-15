// Copyright David Stone 2019.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/algorithms/compare.hpp>
#include <containers/at.hpp>
#include <containers/begin_end.hpp>
#include <containers/common_functions.hpp>
#include <containers/data.hpp>
#include <containers/empty.hpp>
#include <containers/front_back.hpp>
#include <containers/size.hpp>

namespace containers {
namespace detail {
namespace common {

#define CONTAINERS_NON_MODIFYING_COMMON_USING_DECLARATIONS \
	BOUNDED_COMMON_ARITHMETIC \
	using ::containers::begin; \
	using ::containers::end; \
	using ::containers::detail::common::data; \
	using ::containers::detail::common::at; \
	using ::containers::detail::common::front; \
	using ::containers::detail::common::back; \
	using ::containers::detail::common::size; \
	using ::containers::detail::common::empty;

}	// namespace common

CONTAINERS_NON_MODIFYING_COMMON_USING_DECLARATIONS

}	// namespace detail

CONTAINERS_NON_MODIFYING_COMMON_USING_DECLARATIONS

}	// namespace containers
