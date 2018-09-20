// Copyright David Stone 2016.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/algorithms/advance.hpp>
#include <containers/begin_end.hpp>
#include <containers/empty.hpp>
#include <containers/is_iterable.hpp>

#include <bounded/detail/forward.hpp>
#include <bounded/integer.hpp>

#include <cassert>
#include <utility>

namespace containers {
namespace detail {
namespace common {

// TODO: noexcept should take into account return value
template<typename Iterable, BOUNDED_REQUIRES(is_iterable<Iterable>)>
constexpr decltype(auto) front(Iterable && iterable) noexcept(never_empty<Iterable>) {
	assert(!empty(iterable));
	return *begin(BOUNDED_FORWARD(iterable));
}
template<typename Iterable, BOUNDED_REQUIRES(is_iterable<Iterable>)>
constexpr decltype(auto) back(Iterable && iterable) noexcept(never_empty<Iterable>) {
	assert(!empty(iterable));
	return *containers::prev(end(BOUNDED_FORWARD(iterable)));
}

}	// namespace common

using ::containers::detail::common::front;
using ::containers::detail::common::back;

}	// namespace detail

using ::containers::detail::common::front;
using ::containers::detail::common::back;

}	// namespace containers
