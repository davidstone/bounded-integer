// Copyright David Stone 2016.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/algorithms/advance.hpp>
#include <containers/begin_end.hpp>
#include <containers/empty.hpp>
#include <containers/is_range.hpp>

#include <bounded/detail/forward.hpp>
#include <bounded/integer.hpp>

#include <cassert>
#include <utility>

namespace containers {
namespace detail {
namespace common {

// TODO: noexcept should take into account return value
template<typename Range, BOUNDED_REQUIRES(is_range<Range>)>
constexpr decltype(auto) front(Range && range) noexcept(never_empty<Range>) {
	assert(!empty(range));
	return *begin(BOUNDED_FORWARD(range));
}
template<typename Range, BOUNDED_REQUIRES(is_range<Range>)>
constexpr decltype(auto) back(Range && range) noexcept(never_empty<Range>) {
	assert(!empty(range));
	return *containers::prev(end(BOUNDED_FORWARD(range)));
}

}	// namespace common

using ::containers::detail::common::front;
using ::containers::detail::common::back;

}	// namespace detail

using ::containers::detail::common::front;
using ::containers::detail::common::back;

}	// namespace containers
