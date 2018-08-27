// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/algorithms/compare.hpp>
#include <containers/algorithms/copy.hpp>
#include <containers/algorithms/distance.hpp>
#include <containers/algorithms/remove.hpp>
#include <containers/algorithms/uninitialized.hpp>
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
#include <containers/is_container.hpp>
#include <containers/is_iterator.hpp>
#include <containers/push_back.hpp>
#include <containers/repeat_n.hpp>
#include <containers/resize.hpp>
#include <containers/size.hpp>

#include <bounded/detail/forward.hpp>
#include <bounded/integer.hpp>

#include <algorithm>
#include <cassert>
#include <initializer_list>
#include <iterator>
#include <utility>

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

template<typename Container>
constexpr auto iterator_points_into_container(Container const & container, typename Container::const_iterator it) {
	return (begin(container) <= it) and (it <= end(container));
}


// Assumes there is enough capacity -- iterators remain valid
// TODO: exception safety
template<typename Container, typename Allocator, typename... Args>
constexpr auto emplace_in_middle_no_reallocation(Container & container, typename Container::const_iterator const position_, Allocator && allocator, Args && ... args) {
	assert(container.capacity() > size(container));
	auto const position = ::containers::detail::mutable_iterator(container, position_);
	auto const original_end = end(container);
	assert(position != original_end);
	container.emplace_back(std::move(back(container)));
	::containers::move_backward(position, std::prev(original_end), original_end);
	auto const pointer = std::addressof(*position);
	::containers::detail::destroy(allocator, pointer);
	::containers::detail::construct(allocator, pointer, BOUNDED_FORWARD(args)...);
}


// TODO: exception safety
// TODO: Check if the range lies within the container
template<typename Container, typename Allocator, typename ForwardIterator, typename Sentinel, typename Function>
constexpr auto insert_impl(Container & container, Allocator && allocator, typename Container::const_iterator position, ForwardIterator first, Sentinel last, Function reallocating_insert) {
	assert(iterator_points_into_container(container, position));
	if (position == end(container)) {
		return append(container, first, last);
	}

	auto const range_size = bounded::throw_policy<std::out_of_range>{}.assignment(
		::containers::distance(first, last),
		0_bi,
		Container::size_type::max()
	);
	if (size(container) + range_size <= container.capacity()) {
		auto const distance_to_end = typename Container::size_type(end(container) - position, bounded::non_check);
		auto const mutable_position = ::containers::detail::mutable_iterator(container, position);
		::containers::uninitialized_move_backward(mutable_position, end(container), end(container) + range_size, allocator);
		auto const remainder = ::containers::copy_n(first, bounded::min(range_size, distance_to_end), mutable_position);
		::containers::uninitialized_copy(remainder.input, last, remainder.output, allocator);
		container.append_from_capacity(range_size);
		return mutable_position;
	}
	
	return reallocating_insert(range_size);
}

}	// namespace detail

CONTAINERS_COMMON_USING_DECLARATIONS

}	// namespace containers
