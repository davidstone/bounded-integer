// Copyright David Stone 2016.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/algorithms/copy.hpp>
#include <containers/algorithms/equal.hpp>
#include <containers/algorithms/iterator.hpp>
#include <containers/algorithms/lexicographical_compare.hpp>
#include <containers/algorithms/remove.hpp>
#include <containers/algorithms/uninitialized.hpp>
#include <containers/common_functions.hpp>
#include <containers/is_container.hpp>
#include <containers/is_iterator.hpp>
#include <containers/repeat_n.hpp>

#include <bounded/integer.hpp>

#include <algorithm>
#include <cassert>
#include <initializer_list>
#include <iterator>
#include <utility>

namespace containers {
namespace detail {
namespace common {

// Provide a free function begin that resolves to member begin if the user has
// defined one with the rvalue qualifier, otherwise default to a definition
// based on the lvalue begin function.
//
// I would like to define containers::begin and containers::end to forward to
// member begin and end if it is defined, but then unqualified calls to begin
// and end become ambiguous with std::begin and std::end. I would be happy with
// std::begin and std::end except they do not have a noexcept specifier.
//
// It would be nice if we could define mutable begin from const begin, and at
// first it seems possible to do so at least for containers with contiguous
// iterators. It would look something like:
//
#if 0
return Container::iterator(
	const_cast<Container::value_type *>(pointer_from(add_const(container))),
	iterator_constructor
);
#endif
//
// However, this assumes that a mutable container implies mutable access to the
// elements in the container. This is not necessarily the case, making this
// generic function dangerous for some types. It is better to accept the burden
// of defining essentially the same function (const and mutable begin and end)
// for types that want to allow mutable access to the elements.

template<typename Return, typename T>
void rvalue_ref_qualified(Return (T::*)() &&);

#define CONTAINERS_DETAIL_BEGIN_END_OVERLOADS(begin_or_end) \
\
using std::begin_or_end; \
\
\
template<typename T, typename Enable = void> \
struct has_rvalue_c_ ## begin_or_end : std::false_type {}; \
\
template<typename T> \
struct has_rvalue_c_ ## begin_or_end<T, decltype(rvalue_ref_qualified(&T::begin_or_end))> : std::true_type {}; \
\
template<typename T> \
constexpr bool has_rvalue_ ## begin_or_end = has_rvalue_c_ ## begin_or_end<T>::value; \
\
\
template<typename Container, BOUNDED_REQUIRES( \
	is_container<Container> and \
	std::is_rvalue_reference<Container &&>::value and \
	has_rvalue_ ## begin_or_end<Container> \
)> \
constexpr auto begin_or_end(Container && container) BOUNDED_NOEXCEPT_VALUE( \
	std::move(container).begin_or_end() \
) \
\
template<typename Container, BOUNDED_REQUIRES( \
	is_container<Container> and \
	std::is_rvalue_reference<Container &&>::value and \
	!has_rvalue_ ## begin_or_end<std::decay_t<Container>> \
)> \
constexpr auto begin_or_end(Container && container) BOUNDED_NOEXCEPT_VALUE( \
	::containers::move_iterator(begin_or_end(container)) \
)

CONTAINERS_DETAIL_BEGIN_END_OVERLOADS(begin)
CONTAINERS_DETAIL_BEGIN_END_OVERLOADS(end)



template<typename Container, BOUNDED_REQUIRES(is_container<Container>)>
constexpr auto cbegin(Container const & container) BOUNDED_NOEXCEPT_VALUE(
	begin(container)
)
template<typename Container, BOUNDED_REQUIRES(is_container<Container>)>
constexpr auto cend(Container const & container) BOUNDED_NOEXCEPT_VALUE(
	end(container)
)



template<typename Container, BOUNDED_REQUIRES(is_container<Container>)>
constexpr auto rbegin(Container && container) BOUNDED_NOEXCEPT_VALUE(
	::containers::reverse_iterator(begin(std::forward<Container>(container)))
)
template<typename Container, BOUNDED_REQUIRES(is_container<Container>)>
constexpr auto rend(Container && container) BOUNDED_NOEXCEPT_VALUE(
	::containers::reverse_iterator(end(std::forward<Container>(container)))
)



template<typename Container, BOUNDED_REQUIRES(is_container<Container>)>
constexpr auto crbegin(Container const & container) BOUNDED_NOEXCEPT_VALUE(
	::containers::reverse_iterator(begin(container))
)
template<typename Container, BOUNDED_REQUIRES(is_container<Container>)>
constexpr auto crend(Container const & container) BOUNDED_NOEXCEPT_VALUE(
	::containers::reverse_iterator(end(container))
)


}	// namespace common


template<typename Container, typename Iterator, BOUNDED_REQUIRES(is_container<Container> and is_iterator<Iterator>)>
constexpr auto mutable_iterator(Container & container, Iterator const it) BOUNDED_NOEXCEPT_VALUE(
	begin(container) + (it - begin(container))
)


namespace common {

template<typename Container, BOUNDED_REQUIRES(is_container<Container>)>
constexpr auto data(Container && container) BOUNDED_NOEXCEPT_DECLTYPE(
	pointer_from(begin(container))
)


template<typename Container, typename Index, BOUNDED_REQUIRES(is_container<Container>)>
constexpr decltype(auto) at(Container && container, Index const index) BOUNDED_NOEXCEPT(
	std::forward<Container>(container)[typename index_type<Container>::overflow_policy{}.assignment(
		bounded::make(index),
		0_bi,
		size(container) - 1_bi
	)]
)

template<typename Container, typename Index, BOUNDED_REQUIRES(is_container<Container>)>
constexpr decltype(auto) at(Container && container, Index const index, bounded::non_check_t) BOUNDED_NOEXCEPT(
	std::forward<Container>(container)[static_cast<index_type<Container>>(index)]
)


template<typename Container>
constexpr auto never_empty = std::numeric_limits<typename std::remove_reference_t<Container>::size_type>::min() > 0_bi;


template<typename Container, BOUNDED_REQUIRES(is_container<Container> and bounded::is_bounded_integer<typename Container::size_type>)>
constexpr auto size(Container const & container) noexcept {
	return typename Container::size_type(end(container) - begin(container), bounded::non_check);
}
template<typename Container, BOUNDED_REQUIRES(is_container<Container> and !bounded::is_bounded_integer<typename Container::size_type>)>
constexpr auto size(Container const & container) noexcept {
	return typename Container::size_type(end(container) - begin(container));
}

template<typename Container, BOUNDED_REQUIRES(is_container<Container>)>
constexpr auto empty(Container const & container) noexcept {
	// The never_empty check is not needed for correctness, but allows this
	// function to be constexpr in more situations.
	return never_empty<Container> ? false : begin(container) == end(container);
}


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


template<typename Container, BOUNDED_REQUIRES(is_container<Container>)>
constexpr auto push_back(Container & container, typename Container::value_type const & value) BOUNDED_NOEXCEPT(
	container.emplace_back(value)
)
template<typename Container, BOUNDED_REQUIRES(is_container<Container>)>
constexpr auto push_back(Container & container, typename Container::value_type && value) BOUNDED_NOEXCEPT(
	container.emplace_back(std::move(value))
)



// TODO: specialize for ForwardIterator to call reserve
template<typename Container, typename InputIterator, typename Sentinel, BOUNDED_REQUIRES(is_container<Container> and is_iterator_sentinel<InputIterator, Sentinel>)>
constexpr auto append(Container & container, InputIterator first, Sentinel last) {
	auto const offset = size(container);
	for (; first != last; ++first) {
		container.emplace_back(*first);
	}
	return begin(container) + offset;
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
	container.insert(position, begin(init), end(init))
)
// TODO: noexcept
template<typename Container, typename Size, BOUNDED_REQUIRES(is_container<Container> and std::numeric_limits<Size>::is_integer)>
constexpr auto insert(Container & container, typename Container::const_iterator const position, Size const count, typename Container::value_type const & value) {
	auto const range = ::containers::detail::repeat_n(count, value);
	return container.insert(position, begin(range), end(range));
}


// TODO: conditional noexcept
template<typename Container, typename Iterator, BOUNDED_REQUIRES(is_container<Container> and is_iterator<Iterator>)>
constexpr auto erase(Container & container, Iterator const first_, Iterator const last_) noexcept {
	auto const first = ::containers::detail::mutable_iterator(container, first_);
	auto const last = ::containers::detail::mutable_iterator(container, last_);
	auto const to_clear = ::containers::move(last, end(container), first).output;
	while (to_clear != end(container)) {
		container.pop_back();
	}
}
template<typename Container, BOUNDED_REQUIRES(is_container<Container>)>
constexpr auto erase(Container & container, typename Container::const_iterator const it) {
	assert(it != end(container));
	erase(container, it, ::containers::next(it));
}

template<typename Container, typename Predicate, BOUNDED_REQUIRES(is_container<Container>)>
constexpr auto erase_if(Container & container, Predicate predicate) {
	erase(container, ::containers::remove_if(begin(container), end(container), std::move(predicate)), end(container));
}


// TODO: noexcept
template<typename Container, typename InputIterator, typename Sentinel, BOUNDED_REQUIRES(is_container<Container> and is_iterator_sentinel<InputIterator, Sentinel>)>
constexpr auto assign(Container & container, InputIterator first, Sentinel const last) {
	// TODO: Do we try to reuse storage like this or just clear() + construct
	auto it = begin(container);
	for (; first != last; ++first) {
		if (it == end(container)) {
			break;
		}
		*it = *first;
		++it;
	}
	erase(container, it, end(container));
	for (; first != last; ++first) {
		container.emplace_back(*first);
	}
}
template<typename Container, BOUNDED_REQUIRES(is_container<Container>)>
constexpr auto assign(Container & container, std::initializer_list<typename Container::value_type> init) BOUNDED_NOEXCEPT(
	assign(container, begin(init), end(init))
)
template<typename Container, typename Size, BOUNDED_REQUIRES(is_container<Container> and std::numeric_limits<Size>::is_integer)>
constexpr auto assign(Container & container, Size const count, typename Container::value_type const & value) {
	auto const range = ::containers::detail::repeat_n(count, value);
	assign(container, begin(range), end(range));
}


template<typename Container, BOUNDED_REQUIRES(is_container<Container>)>
constexpr auto clear(Container & container) noexcept {
	container = Container{};
}

}	// namespace common

struct common_resize_tag{};
template<typename Container, typename Size, typename... MaybeInitializer>
constexpr auto resize(common_resize_tag, Container & container, Size const count, MaybeInitializer && ... args) {
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
constexpr auto resize(Container & container, Size const count) BOUNDED_NOEXCEPT(
	resize(common_resize_tag{}, container, count)
)
template<typename Container, typename Size, BOUNDED_REQUIRES(is_container<Container>)>
constexpr auto resize(Container & container, Size const count, typename Container::value_type const & value) BOUNDED_NOEXCEPT(
	resize(common_resize_tag{}, container, count, value)
)


template<typename Container, BOUNDED_REQUIRES(is_container<Container>)>
constexpr auto operator==(Container const & lhs, Container const & rhs) BOUNDED_NOEXCEPT(
	::containers::equal(begin(lhs), end(lhs), begin(rhs), end(rhs))
)

template<typename Container, BOUNDED_REQUIRES(is_container<Container>)>
constexpr auto operator<(Container const & lhs, Container const & rhs) BOUNDED_NOEXCEPT(
	::containers::lexicographical_compare(begin(lhs), end(lhs), begin(rhs), end(rhs))
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
	using ::containers::detail::common::resize; \
	using ::containers::detail::common::operator==; \
	using ::containers::detail::common::operator<; \
	CONTAINERS_DETAIL_COMMON_USING_DECLARATIONS

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
	::containers::move_backward(position, ::containers::prev(original_end), original_end);
	auto const pointer = ::bounded::addressof(*position);
	::containers::detail::destroy(allocator, pointer);
	::containers::detail::construct(allocator, pointer, std::forward<Args>(args)...);
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
