// Functions common to most containers
// Copyright (C) 2015 David Stone
//
// This program is free software: you can redistribute it and / or modify
// it under the terms of the GNU Affero General Public License as
// published by the Free Software Foundation, either version 3 of the
// License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU Affero General Public License for more details.
//
// You should have received a copy of the GNU Affero General Public License
// along with this program. If not, see <http://www.gnu.org/licenses/>.

#pragma once

#include <containers/algorithms/copy.hpp>
#include <containers/common_functions.hpp>
#include <containers/is_container.hpp>
#include <containers/is_iterator.hpp>
#include <containers/repeat_n.hpp>

#include <bounded_integer/bounded_integer.hpp>

#include <algorithm>
#include <cassert>
#include <initializer_list>
#include <iterator>
#include <utility>

namespace containers {
namespace detail {

template<typename Container, BOUNDED_REQUIRES(is_container<Container>)>
constexpr auto make_mutable_iterator(Container & container, typename Container::const_iterator it) {
	return container.begin() + (it - container.begin());
}

namespace common {

template<typename Container, BOUNDED_REQUIRES(is_container<Container>)>
constexpr auto begin(Container && container) BOUNDED_NOEXCEPT(
	std::forward<Container>(container).begin()
)
template<typename Container, BOUNDED_REQUIRES(is_container<Container>)>
constexpr auto end(Container && container) BOUNDED_NOEXCEPT(
	std::forward<Container>(container).end()
)

template<typename Container, BOUNDED_REQUIRES(is_container<Container>)>
constexpr auto cbegin(Container const & container) BOUNDED_NOEXCEPT(
	container.begin()
)
template<typename Container, BOUNDED_REQUIRES(is_container<Container>)>
constexpr auto cend(Container const & container) BOUNDED_NOEXCEPT(
	container.end()
)

template<typename Container, BOUNDED_REQUIRES(is_container<Container>)>
constexpr auto rbegin(Container && container) BOUNDED_NOEXCEPT(
	std::make_reverse_iterator(std::forward<Container>(container).begin())
)
template<typename Container, BOUNDED_REQUIRES(is_container<Container>)>
constexpr auto rend(Container && container) BOUNDED_NOEXCEPT(
	std::make_reverse_iterator(std::forward<Container>(container).end())
)

template<typename Container, BOUNDED_REQUIRES(is_container<Container>)>
constexpr auto crbegin(Container const & container) BOUNDED_NOEXCEPT(
	std::make_reverse_iterator(container.begin())
)
template<typename Container, BOUNDED_REQUIRES(is_container<Container>)>
constexpr auto crend(Container const & container) BOUNDED_NOEXCEPT(
	std::make_reverse_iterator(container.end())
)


template<typename Container, typename Index, BOUNDED_REQUIRES(is_container<Container>)>
constexpr decltype(auto) at(Container && container, Index const index) BOUNDED_NOEXCEPT(
	std::forward<Container>(container)[typename index_type<Container>::overflow_policy{}.assignment(
		index,
		bounded::constant<0>,
		size(container) - bounded::constant<1>
	)]
)


template<typename Container>
constexpr auto never_empty = std::numeric_limits<typename std::remove_reference_t<Container>::size_type>::min() > bounded::constant<0>;


template<typename Container, BOUNDED_REQUIRES(is_container<Container>)>
constexpr auto size(Container const & container) noexcept {
	return typename Container::size_type(container.end() - container.begin(), bounded::non_check);
}

template<typename Container, BOUNDED_REQUIRES(is_container<Container>)>
constexpr auto empty(Container const & container) noexcept {
	// The never_empty check is not needed for correctness, but allows this
	// function to be constexpr in more situations.
	return never_empty<Container> ? false : container.begin() == container.end();
}

template<typename Container, BOUNDED_REQUIRES(is_container<Container>)>
constexpr auto max_size() noexcept {
	return std::numeric_limits<typename Container::size_type>::max();
}


template<typename Container, BOUNDED_REQUIRES(is_container<Container>)>
constexpr decltype(auto) front(Container && container) noexcept(never_empty<Container>) {
	assert(!empty(container));
	return *std::forward<Container>(container).begin();
}
template<typename Container, BOUNDED_REQUIRES(is_container<Container>)>
constexpr decltype(auto) back(Container && container) noexcept(never_empty<Container>) {
	assert(!empty(container));
	return *bounded::prev(std::forward<Container>(container).end());
}


template<typename Container, BOUNDED_REQUIRES(is_container<Container>)>
constexpr auto push_back(Container & container, typename Container::value_type const & value) BOUNDED_NOEXCEPT(
	container.emplace_back(value)
)
template<typename Container, BOUNDED_REQUIRES(is_container<Container>)>
constexpr auto push_back(Container & container, typename Container::value_type && value) BOUNDED_NOEXCEPT(
	container.emplace_back(std::move(value))
)



// TODO: specialize for ForwardIterator to call reserve
template<typename Container, typename InputIterator, typename Sentinel, BOUNDED_REQUIRES(is_container<Container> and is_iterator<InputIterator>)>
auto append(Container & container, InputIterator first, Sentinel last) {
	auto const offset = size(container);
	for (; first != last; ++first) {
		container.emplace_back(*first);
	}
	return container.begin() + offset;
}



template<typename Container, BOUNDED_REQUIRES(is_container<Container>)>
constexpr auto insert(Container & container, typename Container::const_iterator const position, typename Container::value_type const & value) BOUNDED_NOEXCEPT(
	container.emplace(position, value)
)
template<typename Container, BOUNDED_REQUIRES(is_container<Container>)>
constexpr auto insert(Container & container, typename Container::const_iterator const position, typename Container::value_type && value) BOUNDED_NOEXCEPT(
	container.emplace(position, std::move(value))
)
template<typename Container, BOUNDED_REQUIRES(is_container<Container>)>
constexpr auto insert(Container & container, typename Container::const_iterator const position, std::initializer_list<typename Container::value_type> init) BOUNDED_NOEXCEPT(
	container.insert(position, init.begin(), init.end())
)
// TODO: noexcept
template<typename Container, typename Size, BOUNDED_REQUIRES(is_container<Container> and std::numeric_limits<Size>::is_integer)>
constexpr auto insert(Container & container, typename Container::const_iterator const position, Size const count, typename Container::value_type const & value) {
	auto const range = ::containers::detail::repeat_n(count, value);
	return container.insert(position, range.begin(), range.end());
}


// TODO: conditional noexcept
template<typename Container, BOUNDED_REQUIRES(is_container<Container>)>
constexpr auto erase(Container & container, typename Container::const_iterator const first_, typename Container::const_iterator const last_) noexcept {
	auto const first = ::containers::detail::make_mutable_iterator(container, first_);
	auto const last = ::containers::detail::make_mutable_iterator(container, last_);
	auto const to_clear = std::move(last, container.end(), first);
	while (to_clear != container.end()) {
		container.pop_back();
	}
}
template<typename Container, BOUNDED_REQUIRES(is_container<Container>)>
constexpr auto erase(Container & container, typename Container::const_iterator const it) {
	assert(it != container.end());
	erase(container, it, bounded::next(it));
}


// TODO: noexcept
template<typename Container, typename InputIterator, typename Sentinel, BOUNDED_REQUIRES(is_container<Container> and is_iterator<InputIterator>)>
constexpr auto assign(Container & container, InputIterator first, Sentinel const last) {
	// TODO: Do we try to reuse storage like this or just clear() + construct
	auto it = container.begin();
	for (; first != last; ++first) {
		if (it == container.end()) {
			break;
		}
		*it = *first;
		++it;
	}
	erase(container, it, container.end());
	for (; first != last; ++first) {
		container.emplace_back(*first);
	}
}
template<typename Container, BOUNDED_REQUIRES(is_container<Container>)>
constexpr auto assign(Container & container, std::initializer_list<typename Container::value_type> init) BOUNDED_NOEXCEPT(
	assign(container, init.begin(), init.end())
)
template<typename Container, typename Size, BOUNDED_REQUIRES(is_container<Container> and std::numeric_limits<Size>::is_integer)>
constexpr auto assign(Container & container, Size const count, typename Container::value_type const & value) {
	auto const range = ::containers::detail::repeat_n(count, value);
	assign(container, range.begin(), range.end());
}


template<typename Container, BOUNDED_REQUIRES(is_container<Container>)>
constexpr auto clear(Container & container) noexcept {
	container = Container{};
}

}	// namespace common

struct common_resize_tag{};
template<typename Container, typename Size, typename... MaybeInitializer>
auto resize(common_resize_tag, Container & container, Size const count, MaybeInitializer && ... args) {
	static_assert(sizeof...(MaybeInitializer) == 0 or sizeof...(MaybeInitializer) == 1);
	while (size(container) > count) {
		container.pop_back();
	}
	while (size(container) < count) {
		container.emplace_back(std::forward<MaybeInitializer>(args)...);
	}
}

namespace common {

template<typename Container, typename Size, BOUNDED_REQUIRES(is_container<Container>)>
auto resize(Container & container, Size const count) BOUNDED_NOEXCEPT(
	resize(common_resize_tag{}, container, count)
)
template<typename Container, typename Size, BOUNDED_REQUIRES(is_container<Container>)>
auto resize(Container & container, Size const count, typename Container::value_type const & value) BOUNDED_NOEXCEPT(
	resize(common_resize_tag{}, container, count, value)
)

	
template<typename Container, BOUNDED_REQUIRES(is_container<Container>)>
constexpr auto operator==(Container const & lhs, Container const & rhs) BOUNDED_NOEXCEPT(
	std::equal(lhs.begin(), lhs.end(), rhs.begin(), rhs.end())
)

template<typename Container, BOUNDED_REQUIRES(is_container<Container>)>
constexpr auto operator<(Container const & lhs, Container const & rhs) BOUNDED_NOEXCEPT(
	std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end())
)

#define CONTAINERS_COMMON_USING_DECLARATIONS \
	using ::containers::detail::common::begin; \
	using ::containers::detail::common::end; \
	using ::containers::detail::common::cbegin; \
	using ::containers::detail::common::cend; \
	using ::containers::detail::common::rbegin; \
	using ::containers::detail::common::rend; \
	using ::containers::detail::common::crbegin; \
	using ::containers::detail::common::crend; \
	using ::containers::detail::common::at; \
	using ::containers::detail::common::front; \
	using ::containers::detail::common::back; \
	using ::containers::detail::common::size; \
	using ::containers::detail::common::empty; \
	using ::containers::detail::common::max_size; \
	using ::containers::detail::common::push_back; \
	using ::containers::detail::common::append; \
	using ::containers::detail::common::insert; \
	using ::containers::detail::common::erase; \
	using ::containers::detail::common::assign; \
	using ::containers::detail::common::clear; \
	using ::containers::detail::common::resize; \
	using ::containers::detail::common::operator==; \
	using ::containers::detail::common::operator<;

}	// namespace common

CONTAINERS_COMMON_USING_DECLARATIONS

template<typename Allocator, typename T, typename... Args>
constexpr auto construct(Allocator && allocator, T * pointer, Args && ... args) BOUNDED_NOEXCEPT(
	std::allocator_traits<std::decay_t<Allocator>>::construct(allocator, reinterpret_cast<typename std::decay_t<Allocator>::value_type *>(pointer), std::forward<Args>(args)...)
)
template<typename Allocator, typename T>
constexpr auto destroy(Allocator && allocator, T * pointer) BOUNDED_NOEXCEPT(
	std::allocator_traits<std::decay_t<Allocator>>::destroy(allocator, reinterpret_cast<typename std::decay_t<Allocator>::value_type *>(pointer))
)

// Assumes there is enough capacity -- iterators remain valid
// TODO: exception safety
template<typename Container, typename Allocator, typename... Args>
auto emplace_in_middle_no_reallocation(Container & container, typename Container::const_iterator const position_, Allocator && allocator, Args && ... args) {
	auto const position = detail::make_mutable_iterator(container, position_);
	auto const original_end = container.end();
	container.emplace_back(std::move(back(container)));
	std::move_backward(position, bounded::prev(original_end), original_end);
	auto const pointer = std::addressof(*position);
	destroy(allocator, pointer);
	construct(allocator, pointer, std::forward<Args>(args)...);
}


// Assumes there is enough capacity -- iterators remain valid
// Container must update its own size
// TODO: exception safety
template<typename Container, typename ForwardIterator, typename Sentinel, typename Size, typename Allocator>
auto put_in_middle_no_reallocation(Container & container, typename Container::const_iterator const position, ForwardIterator first, Sentinel const last, Size const range_size, Allocator && allocator) {
	auto const distance_to_end = typename Container::size_type(container.end() - position, bounded::non_check);
	auto const mutable_position = make_mutable_iterator(container, position);
	detail::uninitialized_move_backward(mutable_position, container.end(), container.end() + range_size, allocator);
	auto const remainder = detail::copy_n(first, bounded::min(range_size, distance_to_end), mutable_position);
	detail::uninitialized_copy(remainder.input, last, remainder.output, allocator);
	return mutable_position;
}

}	// namespace detail

CONTAINERS_COMMON_USING_DECLARATIONS

#undef CONTAINERS_COMMON_USING_DECLARATIONS

}	// namespace containers
