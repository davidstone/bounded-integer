// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/algorithms/compare.hpp>
#include <containers/algorithms/copy.hpp>
#include <containers/algorithms/copy_n.hpp>
#include <containers/algorithms/distance.hpp>
#include <containers/algorithms/uninitialized.hpp>
#include <containers/append.hpp>
#include <containers/begin_end.hpp>
#include <containers/common_functions.hpp>
#include <containers/emplace_back.hpp>
#include <containers/front_back.hpp>
#include <containers/mutable_iterator.hpp>
#include <containers/size.hpp>

#include <bounded/detail/forward.hpp>
#include <bounded/assert.hpp>
#include <bounded/integer.hpp>

#include <iterator>
#include <stdexcept>
#include <utility>

namespace containers {
namespace detail {

template<typename Container>
constexpr auto iterator_points_into_container(Container const & container, typename Container::const_iterator it) {
	return (begin(container) <= it) and (it <= end(container));
}


// TODO: exception safety
template<typename Container, typename Function, typename... Args>
constexpr auto emplace_impl(Container & container, typename Container::const_iterator const position, Function reallocating_emplace, Args && ... args) {
	BOUNDED_ASSERT(::containers::detail::iterator_points_into_container(container, position));
	auto const offset = position - begin(container);
	if (position == end(container)) {
		::containers::emplace_back(container, BOUNDED_FORWARD(args)...);
	} else if (size(container) < container.capacity()) {
		auto const mutable_position = detail::mutable_iterator(container, position);
		auto const original_end = end(container);
		::containers::emplace_back(container, std::move(back(container)));
		::containers::move_backward(mutable_position, containers::prev(original_end), original_end);
		auto const pointer = std::addressof(*mutable_position);
		bounded::destroy(*pointer);
		bounded::construct(*pointer, BOUNDED_FORWARD(args)...);
	} else {
		reallocating_emplace();
	}
	return begin(container) + offset;
}


}	// namespace detail
}	// namespace containers
