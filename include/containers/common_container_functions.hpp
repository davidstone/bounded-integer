// Copyright David Stone 2016.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/algorithms/compare.hpp>
#include <containers/algorithms/copy.hpp>
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


// Assumes there is enough capacity -- iterators remain valid
// Container must update its own size
// TODO: exception safety
template<typename Container, typename ForwardIterator, typename Sentinel, typename Size, typename Allocator>
constexpr auto put_in_middle_no_reallocation(Container & container, typename Container::const_iterator const position, ForwardIterator first, Sentinel const last, Size const range_size, Allocator && allocator) {
	auto const distance_to_end = typename Container::size_type(end(container) - position, bounded::non_check);
	auto const mutable_position = ::containers::detail::mutable_iterator(container, position);
	::containers::uninitialized_move_backward(mutable_position, end(container), end(container) + range_size, allocator);
	auto const remainder = ::containers::copy_n(first, bounded::min(range_size, distance_to_end), mutable_position);
	::containers::uninitialized_copy(remainder.input, last, remainder.output, allocator);
	return mutable_position;
}

}	// namespace detail

CONTAINERS_COMMON_USING_DECLARATIONS

}	// namespace containers
