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

#include <containers/repeat_n.hpp>

#include <bounded_integer/bounded_integer.hpp>

#include <algorithm>
#include <initializer_list>
#include <iterator>
#include <type_traits>
#include <utility>

namespace containers {

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


template<typename Container, typename Index>
constexpr decltype(auto) at(Container && container, Index const index) BOUNDED_NOEXCEPT(
	std::forward<Container>(container)[static_cast<typename std::remove_reference_t<Container>::index_type>(index)]
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

// TODO: max_size ?


template<typename Container>
constexpr auto push_back(Container & container, typename Container::value_type const & value) BOUNDED_NOEXCEPT(
	container.emplace_back(value)
)
template<typename Container>
constexpr auto push_back(Container & container, typename Container::value_type && value) BOUNDED_NOEXCEPT(
	container.emplace_back(std::move(value))
)


// TODO: Work with const_iterator instead of iterator

template<typename Container>
constexpr auto insert(Container & container, typename Container::iterator const position, typename Container::value_type const & value) BOUNDED_NOEXCEPT(
	container.emplace(position, value)
)
template<typename Container>
constexpr auto insert(Container & container, typename Container::iterator const position, typename Container::value_type && value) BOUNDED_NOEXCEPT(
	container.emplace(position, std::move(value))
)
template<typename Container>
constexpr auto insert(Container & container, typename Container::iterator const position, std::initializer_list<typename Container::value_type> init) BOUNDED_NOEXCEPT(
	container.insert(position, init.begin(), init.end())
)
// TODO: noexcept
template<typename Container, typename Size, BOUNDED_REQUIRES(std::numeric_limits<Size>::is_integer)>
constexpr auto insert(Container & container, typename Container::iterator const position, Size const count, typename Container::value_type const & value) {
	auto const range = detail::repeat_n(count, value);
	return container.insert(position, range.begin(), range.end());
}


// TODO: conditional noexcept
template<typename Container>
constexpr auto erase(Container & container, typename Container::iterator const first, typename Container::iterator const last) noexcept {
	auto const to_clear = std::move(last, container.end(), first);
	while (to_clear != container.end()) {
		container.pop_back();
	}
}
template<typename Container>
constexpr auto erase(Container & container, typename Container::iterator const it) {
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
	auto const range = detail::repeat_n(count, value);
	assign(container, range.begin(), range.end());
}


template<typename Container>
constexpr auto clear(Container & container) noexcept {
	erase(container, container.begin(), container.end());
}


namespace detail {

template<typename Container, typename Size, typename... MaybeInitializer>
auto resize(Container & container, Size const count, MaybeInitializer && ... args) {
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
constexpr auto operator!=(Container const & lhs, Container const & rhs) BOUNDED_NOEXCEPT(
	!(lhs == rhs)
)

template<typename Container>
constexpr auto operator<(Container const & lhs, Container const & rhs) BOUNDED_NOEXCEPT(
	std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end())
)
template<typename Container>
auto operator>(Container const & lhs, Container const & rhs) BOUNDED_NOEXCEPT(
	rhs < lhs
)
template<typename Container>
auto operator<=(Container const & lhs, Container const & rhs) BOUNDED_NOEXCEPT(
	!(rhs < lhs)
)
template<typename Container>
auto operator>=(Container const & lhs, Container const & rhs) BOUNDED_NOEXCEPT(
	!(lhs < rhs)
)


}	// namespace containers
