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

#include <containers/common_functions.hpp>
#include <containers/is_container.hpp>
#include <containers/repeat_n.hpp>

#include <bounded_integer/bounded_integer.hpp>

#include <algorithm>
#include <initializer_list>
#include <iterator>
#include <utility>

namespace containers {
namespace detail {

template<typename Container>
constexpr auto make_mutable_iterator(Container & container, typename Container::const_iterator it) {
	return container.begin() + (it - container.begin());
}

namespace common {

template<typename Container>
constexpr auto begin(Container && container) BOUNDED_NOEXCEPT(
	std::forward<Container>(container).begin()
)
template<typename Container>
constexpr auto end(Container && container) BOUNDED_NOEXCEPT(
	std::forward<Container>(container).end()
)

template<typename Container>
constexpr auto cbegin(Container const & container) BOUNDED_NOEXCEPT(
	container.begin()
)
template<typename Container>
constexpr auto cend(Container const & container) BOUNDED_NOEXCEPT(
	container.end()
)

namespace detail {
// TODO: Use std::make_reverse_iterator when I have a newer system header

template<typename Iterator>
constexpr auto make_reverse_iterator(Iterator const it) BOUNDED_NOEXCEPT(
	std::reverse_iterator<Iterator>(it)
)

}	// namespace detail

template<typename Container>
constexpr auto rbegin(Container && container) BOUNDED_NOEXCEPT(
	detail::make_reverse_iterator(std::forward<Container>(container).begin())
)
template<typename Container>
constexpr auto rend(Container && container) BOUNDED_NOEXCEPT(
	detail::make_reverse_iterator(std::forward<Container>(container).end())
)

template<typename Container>
constexpr auto crbegin(Container const & container) BOUNDED_NOEXCEPT(
	detail::make_reverse_iterator(container.begin())
)
template<typename Container>
constexpr auto crend(Container const & container) BOUNDED_NOEXCEPT(
	detail::make_reverse_iterator(container.end())
)


template<typename Container, typename Index, BOUNDED_REQUIRES(is_container<Container>)>
constexpr decltype(auto) at(Container && container, Index const index) BOUNDED_NOEXCEPT(
	std::forward<Container>(container)[static_cast<index_type<Container>>(index)]
)


// These can be declared noexcept in more situations. For instance, these are
// noexcept for array if container.size() > 0
template<typename Container>
constexpr decltype(auto) front(Container && container) BOUNDED_NOEXCEPT(
	*std::forward<Container>(container).begin()
)
template<typename Container>
constexpr decltype(auto) back(Container && container) BOUNDED_NOEXCEPT(
	*bounded::prev(std::forward<Container>(container).end())
)


template<typename Container>
constexpr auto size(Container const & container) noexcept {
	return typename Container::size_type(container.end() - container.begin(), bounded::non_check);
}

template<typename Container>
constexpr auto empty(Container const & container) noexcept {
	return container.begin() == container.end();
}

template<typename Container>
constexpr auto max_size() noexcept {
	return std::numeric_limits<typename Container::size_type>::max();
}



template<typename Container>
constexpr auto push_back(Container & container, typename Container::value_type const & value) BOUNDED_NOEXCEPT(
	container.emplace_back(value)
)
template<typename Container>
constexpr auto push_back(Container & container, typename Container::value_type && value) BOUNDED_NOEXCEPT(
	container.emplace_back(std::move(value))
)



template<typename Container>
constexpr auto insert(Container & container, typename Container::const_iterator const position, typename Container::value_type const & value) BOUNDED_NOEXCEPT(
	container.emplace(position, value)
)
template<typename Container>
constexpr auto insert(Container & container, typename Container::const_iterator const position, typename Container::value_type && value) BOUNDED_NOEXCEPT(
	container.emplace(position, std::move(value))
)
template<typename Container>
constexpr auto insert(Container & container, typename Container::const_iterator const position, std::initializer_list<typename Container::value_type> init) BOUNDED_NOEXCEPT(
	container.insert(position, init.begin(), init.end())
)
// TODO: noexcept
template<typename Container, typename Size, BOUNDED_REQUIRES(std::numeric_limits<Size>::is_integer)>
constexpr auto insert(Container & container, typename Container::const_iterator const position, Size const count, typename Container::value_type const & value) {
	auto const range = ::containers::detail::repeat_n(count, value);
	return container.insert(position, range.begin(), range.end());
}


// TODO: conditional noexcept
template<typename Container>
constexpr auto erase(Container & container, typename Container::const_iterator const first_, typename Container::const_iterator const last_) noexcept {
	auto const first = ::containers::detail::make_mutable_iterator(container, first_);
	auto const last = ::containers::detail::make_mutable_iterator(container, last_);
	auto const to_clear = std::move(last, container.end(), first);
	while (to_clear != container.end()) {
		container.pop_back();
	}
}
template<typename Container>
constexpr auto erase(Container & container, typename Container::const_iterator const it) {
	assert(it != container.end());
	erase(container, it, bounded::next(it));
}


// TODO: noexcept
template<typename Container, typename InputIterator, typename Sentinel, typename = typename std::iterator_traits<InputIterator>::iterator_category>
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
template<typename Container>
constexpr auto assign(Container & container, std::initializer_list<typename Container::value_type> init) BOUNDED_NOEXCEPT(
	assign(container, init.begin(), init.end())
)
template<typename Container, typename Size, BOUNDED_REQUIRES(std::numeric_limits<Size>::is_integer)>
constexpr auto assign(Container & container, Size const count, typename Container::value_type const & value) {
	auto const range = ::containers::detail::repeat_n(count, value);
	assign(container, range.begin(), range.end());
}


template<typename Container>
constexpr auto clear(Container & container) noexcept {
	erase(container, container.begin(), container.end());
}


namespace detail {

template<typename Container, typename Size, typename... MaybeInitializer>
auto resize(Container & container, Size const count, MaybeInitializer && ... args) {
	static_assert(sizeof...(MaybeInitializer) == 0 or sizeof...(MaybeInitializer) == 1);
	while (size(container) > count) {
		container.pop_back();
	}
	while (size(container) < count) {
		container.emplace_back(std::forward<MaybeInitializer>(args)...);
	}
}

}	// namespace detail

template<typename Container, typename Size>
auto resize(Container & container, Size const count) BOUNDED_NOEXCEPT(
	detail::resize(container, count)
)
template<typename Container, typename Size>
auto resize(Container & container, Size const count, typename Container::value_type const & value) BOUNDED_NOEXCEPT(
	detail::resize(container, count, value)
)

	
template<typename Container>
constexpr auto operator==(Container const & lhs, Container const & rhs) BOUNDED_NOEXCEPT(
	std::equal(lhs.begin(), lhs.end(), rhs.begin(), rhs.end())
)

template<typename Container>
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
	using ::containers::detail::common::insert; \
	using ::containers::detail::common::erase; \
	using ::containers::detail::common::assign; \
	using ::containers::detail::common::clear; \
	using ::containers::detail::common::resize; \
	using ::containers::detail::common::operator==; \
	using ::containers::detail::common::operator<;

}	// namespace common

CONTAINERS_COMMON_USING_DECLARATIONS

}	// namespace detail

CONTAINERS_COMMON_USING_DECLARATIONS

#undef CONTAINERS_COMMON_USING_DECLARATIONS

}	// namespace containers
