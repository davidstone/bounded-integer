// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/algorithms/compare.hpp>
#include <containers/append.hpp>
#include <containers/assign.hpp>
#include <containers/at.hpp>
#include <containers/begin_end.hpp>
#include <containers/common_functions.hpp>
#include <containers/clear.hpp>
#include <containers/data.hpp>
#include <containers/empty.hpp>
#include <containers/erase.hpp>
#include <containers/front_back.hpp>
#include <containers/insert.hpp>
#include <containers/push_back.hpp>
#include <containers/resize.hpp>
#include <containers/size.hpp>

namespace containers {
namespace detail {
namespace common {

#define CONTAINERS_COMMON_USING_DECLARATIONS \
	CONTAINERS_DETAIL_COMMON_USING_DECLARATIONS \
	CONTAINERS_COMMON_BEGIN_END_USING_DECLARATIONS \
	using ::containers::detail::common::data; \
	using ::containers::detail::common::at; \
	using ::containers::detail::common::front; \
	using ::containers::detail::common::back; \
	using ::containers::detail::common::size; \
	using ::containers::detail::common::empty; \
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
