// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/algorithms/copy.hpp>
#include <containers/algorithms/copy_n.hpp>
#include <containers/algorithms/uninitialized.hpp>
#include <containers/begin_end.hpp>
#include <containers/count_type.hpp>
#include <containers/data.hpp>
#include <containers/front_back.hpp>
#include <containers/mutable_iterator.hpp>
#include <containers/push_back.hpp>
#include <containers/range_value_t.hpp>
#include <containers/range_view.hpp>
#include <containers/reserve_if_reservable.hpp>
#include <containers/resizable_container.hpp>
#include <containers/size.hpp>

#include <bounded/integer.hpp>
#include <bounded/unreachable.hpp>

#include <utility>

namespace containers {
namespace detail {

template<typename Container>
constexpr auto insert_with_reallocation(Container & container, typename Container::const_iterator const position, auto const number_of_elements, auto construct) {
	// There is a reallocation required, so just put everything in the
	// correct place to begin with
	auto const original_size = containers::size(container);
	auto temp = Container();
	temp.reserve(::containers::detail::reallocation_size(container.capacity(), original_size, number_of_elements));
	// First construct the new element because the arguments to
	// construct it may reference an old element. We cannot move
	// elements it references before constructing it
	auto const offset = position - containers::begin(container);
	construct(containers::data(temp) + offset);
	auto const mutable_position = containers::begin(container) + offset;
	auto const temp_begin = containers::begin(temp);
	auto const it = containers::uninitialized_relocate_no_overlap(
		range_view(containers::begin(container), mutable_position),
		temp_begin
	);
	BOUNDED_ASSERT(temp_begin + offset == it);
	::containers::uninitialized_relocate_no_overlap(
		range_view(mutable_position, containers::end(container)),
		it + static_cast<offset_type<iterator_t<Container>>>(number_of_elements)
	);
	container.append_from_capacity(-original_size);
	temp.append_from_capacity(original_size + number_of_elements);
	container = std::move(temp);
	return mutable_position;
}

template<typename Container>
constexpr auto iterator_points_into_container(Container const & container, typename Container::const_iterator it) {
	return (containers::begin(container) <= it) and (it <= containers::end(container));
}

} // namespace detail

// TODO: exception safety
template<resizable_container Container>
constexpr auto lazy_insert(
	Container & container,
	typename Container::const_iterator const position,
	bounded::construct_function_for<range_value_t<Container>> auto && constructor
) {
	BOUNDED_ASSERT(::containers::detail::iterator_points_into_container(container, position));
	auto const offset = position - containers::begin(container);
	if (position == containers::end(container)) {
		::containers::lazy_push_back(container, OPERATORS_FORWARD(constructor));
	} else if (containers::size(container) < container.capacity()) {
		auto const mutable_position = ::containers::detail::mutable_iterator(container, position);
		auto const original_end = containers::end(container);
		::containers::push_back(container, std::move(containers::back(container)));
		::containers::move_backward(mutable_position, containers::prev(original_end), original_end);
		auto & ref = *mutable_position;
		bounded::destroy(ref);
		bounded::construct(ref, OPERATORS_FORWARD(constructor));
	} else if constexpr (detail::reservable<Container>) {
		::containers::detail::insert_with_reallocation(container, position, 1_bi, [&](auto const ptr) {
			bounded::construct(*ptr, OPERATORS_FORWARD(constructor));
		});
	} else {
		bounded::assert_or_assume_unreachable();
	}
	return containers::begin(container) + offset;
}

template<resizable_container Container>
constexpr auto emplace(Container & container, typename Container::const_iterator const position, auto && ... args) {
	return ::containers::lazy_insert(container, position, [&] {
		return bounded::construct_return<range_value_t<Container>>(OPERATORS_FORWARD(args)...);
	});
}

// TODO: exception safety
// TODO: Check if the range lies within the container
template<resizable_container Container, range Range> requires bounded::convertible_to<range_value_t<Container>, range_value_t<Range>>
constexpr auto insert(Container & container, typename Container::const_iterator position, Range && range) {
	BOUNDED_ASSERT(::containers::detail::iterator_points_into_container(container, position));
	auto const range_size = ::containers::detail::linear_size(range);
	if (containers::size(container) + range_size <= container.capacity()) {
		auto const mutable_position = ::containers::detail::mutable_iterator(container, position);
		containers::uninitialized_relocate(
			containers::reversed(range_view(mutable_position, containers::end(container))),
			containers::reverse_iterator(containers::end(container) + static_cast<count_type<Container>>(range_size))
		);
		containers::uninitialized_copy_no_overlap(OPERATORS_FORWARD(range), mutable_position);
		container.append_from_capacity(range_size);
		return mutable_position;
	} else if constexpr (detail::reservable<Container>) {
		return ::containers::detail::insert_with_reallocation(container, position, range_size, [&](auto const ptr) {
			containers::uninitialized_copy_no_overlap(OPERATORS_FORWARD(range), ptr);
		});
	} else {
		bounded::assert_or_assume_unreachable();
	}
}

template<resizable_container Container>
constexpr auto insert(Container & container, typename Container::const_iterator const position, range_value_t<Container> const & value) {
	return ::containers::lazy_insert(container, position, bounded::value_to_function(value));
}
template<resizable_container Container>
constexpr auto insert(Container & container, typename Container::const_iterator const position, range_value_t<Container> && value) {
	return ::containers::lazy_insert(container, position, bounded::value_to_function(std::move(value)));
}

} // namespace containers
