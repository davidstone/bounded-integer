// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/algorithms/copy.hpp>
#include <containers/algorithms/copy_n.hpp>
#include <containers/algorithms/uninitialized.hpp>
#include <containers/begin_end.hpp>
#include <containers/emplace_back.hpp>
#include <containers/is_container.hpp>
#include <containers/mutable_iterator.hpp>
#include <containers/repeat_n.hpp>
#include <containers/reserve_if_reservable.hpp>

#include <bounded/integer.hpp>

#include <utility>

namespace containers {
namespace detail {

template<typename Container, typename Integer, typename Function>
constexpr auto insert_or_emplace_with_reallocation(Container & container, typename Container::const_iterator const position, Integer const number_of_elements, Function construct) {
	// There is a reallocation required, so just put everything in the
	// correct place to begin with
	auto temp = Container();
	temp.reserve(::containers::detail::reallocation_size(container, number_of_elements));
	// First construct the new element because the arguments to
	// construct it may reference an old element. We cannot move
	// elements it references before constructing it
	auto const offset = position - begin(container);
	construct(data(temp) + offset);
	auto const mutable_position = begin(container) + offset;
	auto const temp_begin = begin(temp);
	auto const it = containers::uninitialized_move_destroy(begin(container), mutable_position, temp_begin);
	BOUNDED_ASSERT(temp_begin + offset == it);
	::containers::uninitialized_move_destroy(mutable_position, end(container), it + number_of_elements);
	container = std::move(temp);
	container.append_from_capacity(number_of_elements);
	return mutable_position;
}

template<typename Container>
constexpr auto iterator_points_into_container(Container const & container, typename Container::const_iterator it) {
	return (begin(container) <= it) and (it <= end(container));
}


namespace common {

// TODO: exception safety
template<typename Container, typename... Args>
constexpr auto emplace(Container & container, typename Container::const_iterator const position, Args && ... args) {
	BOUNDED_ASSERT(::containers::detail::iterator_points_into_container(container, position));
	auto const offset = position - begin(container);
	if (position == end(container)) {
		::containers::emplace_back(container, BOUNDED_FORWARD(args)...);
	} else if (size(container) < container.capacity()) {
		auto const mutable_position = ::containers::detail::mutable_iterator(container, position);
		auto const original_end = end(container);
		::containers::emplace_back(container, std::move(back(container)));
		::containers::move_backward(mutable_position, containers::prev(original_end), original_end);
		auto const pointer = std::addressof(*mutable_position);
		bounded::destroy(*pointer);
		bounded::construct(*pointer, BOUNDED_FORWARD(args)...);
	} else if constexpr (reservable<Container>) {
		insert_or_emplace_with_reallocation(container, position, 1_bi, [&](auto const ptr) {
			bounded::construct(*ptr, BOUNDED_FORWARD(args)...);
		});
	} else {
		BOUNDED_ASSERT_OR_ASSUME(false);
	}
	return begin(container) + offset;
}

// TODO: exception safety
// TODO: Check if the range lies within the container
// TODO: Return an iterator to the first element inserted
template<typename Container, typename Range = std::initializer_list<typename Container::value_type>>
constexpr auto insert(Container & container, typename Container::const_iterator position, Range && range) {
	BOUNDED_ASSERT(iterator_points_into_container(container, position));
	if (position == end(container)) {
		append(container, BOUNDED_FORWARD(range));
		return;
	}

	auto const range_size = bounded::throw_policy<std::out_of_range>{}.assignment(
		::containers::detail::linear_size(range),
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
		// return mutable_position;
	} else if constexpr (reservable<Container>) {
		insert_or_emplace_with_reallocation(position, range_size, [&](auto const ptr) {
			::containers::uninitialized_copy(BOUNDED_FORWARD(range), ptr);
		});
	} else {
		BOUNDED_ASSERT_OR_ASSUME(false);
	}
}

template<container Container>
constexpr auto insert(Container & container, typename Container::const_iterator const position, typename Container::value_type const & value) {
	return ::containers::detail::common::emplace(container, position, value);
}
template<container Container>
constexpr auto insert(Container & container, typename Container::const_iterator const position, typename Container::value_type && value) {
	return ::containers::detail::common::emplace(container, position, std::move(value));
}

}	// namespace common

using ::containers::detail::common::emplace;
using ::containers::detail::common::insert;

}	// namespace detail

using ::containers::detail::common::emplace;
using ::containers::detail::common::insert;

}	// namespace containers
