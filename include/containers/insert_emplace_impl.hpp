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
#include <containers/front_back.hpp>
#include <containers/mutable_iterator.hpp>
#include <containers/size.hpp>

#include <bounded/detail/forward.hpp>
#include <bounded/integer.hpp>

#include <cassert>
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
	assert(::containers::detail::iterator_points_into_container(container, position));
	auto const offset = position - begin(container);
	if (position == end(container)) {
		container.emplace_back(BOUNDED_FORWARD(args)...);
	} else if (size(container) < container.capacity()) {
		auto const mutable_position = detail::mutable_iterator(container, position);
		auto const original_end = end(container);
		container.emplace_back(std::move(back(container)));
		::containers::move_backward(mutable_position, containers::prev(original_end), original_end);
		auto const pointer = std::addressof(*mutable_position);
		bounded::destroy(*pointer);
		bounded::construct(*pointer, BOUNDED_FORWARD(args)...);
	} else {
		reallocating_emplace();
	}
	return begin(container) + offset;
}


// TODO: exception safety
// TODO: Check if the range lies within the container
template<typename Container, typename Range, typename Function>
constexpr auto insert_impl(Container & container, typename Container::const_iterator position, Range && range, Function reallocating_insert) {
	assert(iterator_points_into_container(container, position));
	if (position == end(container)) {
		return append(container, BOUNDED_FORWARD(range));
	}

	auto const range_size = bounded::throw_policy<std::out_of_range>{}.assignment(
		detail::linear_size(range),
		0_bi,
		Container::size_type::max()
	);
	if (size(container) + range_size <= container.capacity()) {
		auto const distance_to_end = typename Container::size_type(end(container) - position, bounded::non_check);
		auto const mutable_position = ::containers::detail::mutable_iterator(container, position);
		::containers::uninitialized_move_backward(mutable_position, end(container), end(container) + range_size);
		auto const first = begin(BOUNDED_FORWARD(range));
		// TODO: use a view over a counted iterator
		auto const remainder = ::containers::copy_n(first, bounded::min(range_size, distance_to_end), mutable_position);
		auto const last = end(BOUNDED_FORWARD(range));
		::containers::uninitialized_copy(remainder.input, last, remainder.output);
		container.append_from_capacity(range_size);
		return mutable_position;
	}
	
	return reallocating_insert(range_size);
}

}	// namespace detail
}	// namespace containers
