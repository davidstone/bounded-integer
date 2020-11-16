// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/algorithms/copy.hpp>
#include <containers/algorithms/copy_n.hpp>
#include <containers/algorithms/uninitialized.hpp>
#include <containers/begin_end.hpp>
#include <containers/front_back.hpp>
#include <containers/mutable_iterator.hpp>
#include <containers/push_back.hpp>
#include <containers/range_view.hpp>
#include <containers/reserve_if_reservable.hpp>
#include <containers/resizable_container.hpp>

#include <bounded/integer.hpp>
#include <bounded/unreachable.hpp>

#include <utility>

namespace containers {
namespace detail {

template<typename Container>
constexpr auto insert_or_emplace_with_reallocation(Container & container, typename Container::const_iterator const position, auto const number_of_elements, auto construct) {
	// There is a reallocation required, so just put everything in the
	// correct place to begin with
	auto const original_size = size(container);
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
	container.append_from_capacity(original_size + number_of_elements);
	return mutable_position;
}

template<typename Container>
constexpr auto iterator_points_into_container(Container const & container, typename Container::const_iterator it) {
	return (begin(container) <= it) and (it <= end(container));
}

} // namespace detail

// TODO: exception safety
template<resizable_container Container>
constexpr auto lazy_insert(
	Container & container,
	typename Container::const_iterator const position,
	bounded::construct_function_for<typename Container::value_type> auto && constructor
) {
	BOUNDED_ASSERT(::containers::detail::iterator_points_into_container(container, position));
	auto const offset = position - begin(container);
	if (position == end(container)) {
		::containers::lazy_push_back(container, OPERATORS_FORWARD(constructor));
	} else if (size(container) < container.capacity()) {
		auto const mutable_position = ::containers::detail::mutable_iterator(container, position);
		auto const original_end = end(container);
		::containers::push_back(container, std::move(containers::back(container)));
		::containers::move_backward(mutable_position, containers::prev(original_end), original_end);
		auto & ref = *mutable_position;
		bounded::destroy(ref);
		bounded::construct(ref, OPERATORS_FORWARD(constructor));
	} else if constexpr (detail::reservable<Container>) {
		::containers::detail::insert_or_emplace_with_reallocation(container, position, 1_bi, [&](auto const ptr) {
			bounded::construct(*ptr, OPERATORS_FORWARD(constructor));
		});
	} else {
		bounded::assert_or_assume_unreachable();
	}
	return begin(container) + offset;
}

template<resizable_container Container>
constexpr auto emplace(Container & container, typename Container::const_iterator const position, auto && ... args) {
	return ::containers::lazy_insert(container, position, [&] {
		return bounded::construct_return<typename Container::value_type>(OPERATORS_FORWARD(args)...);
	});
}

// TODO: exception safety
// TODO: Check if the range lies within the container
template<resizable_container Container, range Range = std::initializer_list<typename Container::value_type>> requires bounded::convertible_to<typename Container::value_type, typename Range::value_type>
constexpr auto insert(Container & container, typename Container::const_iterator position, Range && range) {
	BOUNDED_ASSERT(::containers::detail::iterator_points_into_container(container, position));
	auto const range_size = ::containers::detail::linear_size(range);
	if (size(container) + range_size <= container.capacity()) {
		auto const mutable_position = ::containers::detail::mutable_iterator(container, position);
		::containers::uninitialized_move_destroy(
			containers::reversed(range_view(mutable_position, end(container))),
			containers::reverse_iterator(end(container) + range_size)
		);
		::containers::uninitialized_copy(OPERATORS_FORWARD(range), position);
		container.append_from_capacity(range_size);
		return mutable_position;
	} else if constexpr (detail::reservable<Container>) {
		return ::containers::detail::insert_or_emplace_with_reallocation(container, position, range_size, [&](auto const ptr) {
			::containers::uninitialized_copy(OPERATORS_FORWARD(range), ptr);
		});
	} else {
		bounded::assert_or_assume_unreachable();
	}
}

template<resizable_container Container>
constexpr auto insert(Container & container, typename Container::const_iterator const position, typename Container::value_type const & value) {
	return ::containers::lazy_insert(container, position, bounded::value_to_function(value));
}
template<resizable_container Container>
constexpr auto insert(Container & container, typename Container::const_iterator const position, typename Container::value_type && value) {
	return ::containers::lazy_insert(container, position, bounded::value_to_function(std::move(value)));
}

} // namespace containers
