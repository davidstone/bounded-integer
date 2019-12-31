// Copyright David Stone 2016.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/algorithms/advance.hpp>
#include <containers/begin_end.hpp>
#include <containers/empty.hpp>
#include <containers/is_range.hpp>

#include <bounded/forward.hpp>
#include <bounded/assert.hpp>
#include <bounded/integer.hpp>

#include <utility>

namespace containers {
namespace detail {
namespace common {

constexpr decltype(auto) front(range auto && r) {
	BOUNDED_ASSERT(!empty(r));
	return *begin(BOUNDED_FORWARD(r));
}
constexpr decltype(auto) back(range auto && r) {
	BOUNDED_ASSERT(!empty(r));
	return *containers::prev(end(BOUNDED_FORWARD(r)));
}

}	// namespace common

using ::containers::detail::common::front;
using ::containers::detail::common::back;

}	// namespace detail

using ::containers::detail::common::front;
using ::containers::detail::common::back;

}	// namespace containers
