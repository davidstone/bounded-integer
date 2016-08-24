// Copyright David Stone 2016.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/algorithms/advance.hpp>
#include <containers/begin_end.hpp>
#include <containers/empty.hpp>
#include <containers/is_container.hpp>

#include <bounded/integer.hpp>

#include <cassert>
#include <utility>

namespace containers {
namespace detail {
namespace common {

// TODO: noexcept should take into account return value
template<typename Container, BOUNDED_REQUIRES(is_container<Container>)>
constexpr decltype(auto) front(Container && container) noexcept(never_empty<Container>) {
	assert(!empty(container));
	return *begin(std::forward<Container>(container));
}
template<typename Container, BOUNDED_REQUIRES(is_container<Container>)>
constexpr decltype(auto) back(Container && container) noexcept(never_empty<Container>) {
	assert(!empty(container));
	return *::containers::prev(end(std::forward<Container>(container)));
}

}	// namespace common
}	// namespace detail
}	// namespace containers
