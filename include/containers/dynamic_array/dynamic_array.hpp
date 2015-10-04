// Dynamically-sized array, keeps track of size only
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

#include <containers/array/iterator.hpp>

#include <containers/uninitialized_storage.hpp>
#include <containers/common_container_functions.hpp>
#include <containers/repeat_n.hpp>

#include <iterator>
#include <memory>

namespace containers {
namespace detail {

// TODO: Work with other iterator categories
template<typename InputIterator, typename Sentinel>
constexpr auto distance(InputIterator first, Sentinel const last) BOUNDED_NOEXCEPT(
	last - first
)

template<typename InputIterator, typename Sentinel, typename OutputIterator>
constexpr auto copy(InputIterator first, Sentinel const last, OutputIterator out) {
	for (; first != last; ++first) {
		*out = * first;
		++out;
	}
	return out;
}

template<typename InputIterator, typename Sentinel, typename ForwardIterator>
auto uninitialized_copy(InputIterator first, Sentinel const last, ForwardIterator out) {
	for (; first != last; ++first) {
		::new(static_cast<void *>(std::addressof(*out))) typename std::iterator_traits<ForwardIterator>::value_type(*first);
		++out;
	}
	return out;
}

template<typename T, typename Size>
auto make_uninitialized_array(Size const size) {
	auto temp = std::make_unique<uninitialized_storage<T>[]>(static_cast<std::size_t>(size));
	return std::unique_ptr<T[]>(reinterpret_cast<T *>(temp.release()));
}

}	// namespace detail

template<typename T>
struct dynamic_array {
	using value_type = T;
	using size_type = bounded::integer<0, std::numeric_limits<std::intmax_t>::max() / sizeof(T)>;

	using const_iterator = detail::basic_array_iterator<value_type const, dynamic_array>;
	using iterator = detail::basic_array_iterator<value_type, dynamic_array>;
	
	constexpr dynamic_array() = default;
	
	template<typename Count, BOUNDED_REQUIRES(std::is_convertible<Count, size_type>::value)>
	explicit dynamic_array(Count const count):
		m_data(detail::make_uninitialized_array<value_type>(count)),
		m_size(count)
	{
		for (auto & value : *this) {
			::new(static_cast<void *>(std::addressof(value))) value_type();
		}
	}
	
	template<typename ForwardIterator, typename Sentinel>
	dynamic_array(ForwardIterator first, Sentinel const last):
		m_size(detail::distance(first, last))
	{
		m_data = detail::make_uninitialized_array<value_type>(m_size);
		detail::uninitialized_copy(first, last, m_data.get());
	}
	dynamic_array(std::initializer_list<value_type> init):
		dynamic_array(init.begin(), init.end())
	{
	}
	template<typename Count, BOUNDED_REQUIRES(std::is_convertible<Count, size_type>::value)>
	dynamic_array(Count const count, value_type const & value) {
		auto const range = ::containers::detail::repeat_n(count, value);
		*this = dynamic_array(range.begin(), range.end());
	}
	
	dynamic_array(dynamic_array const & other):
		dynamic_array(other.begin(), other.end())
	{
	}
	constexpr dynamic_array(dynamic_array && other) noexcept = default;

	auto & operator=(dynamic_array const & other) & {
		assign(*this, other.begin(), other.end());
		return *this;
	}
	constexpr auto operator=(dynamic_array && other) & noexcept -> dynamic_array & = default;
	
	constexpr auto data() const noexcept {
		return m_data.get();
	}
	constexpr auto data() noexcept {
		return m_data.get();
	}
	
	constexpr auto begin() const noexcept {
		return const_iterator(data());
	}
	constexpr auto begin() noexcept {
		return iterator(data());
	}
	constexpr auto end() const noexcept {
		return begin() + m_size;
	}
	constexpr auto end() noexcept {
		return begin() + m_size;
	}


	constexpr auto && operator[](index_type<dynamic_array> const index) const noexcept {
		return *(begin() + index);
	}
	constexpr auto && operator[](index_type<dynamic_array> const index) noexcept {
		return *(begin() + index);
	}
	
private:
	std::unique_ptr<value_type[]> m_data = nullptr;
	size_type m_size = bounded::constant<0>;
};


template<typename T, typename ForwardIterator, typename Sentinel, typename = typename std::iterator_traits<ForwardIterator>::iterator_category>
auto assign(dynamic_array<T> & container, ForwardIterator first, Sentinel const last) {
	auto const difference = detail::distance(first, last);
	if (difference == size(container)) {
		detail::copy(first, last, container.begin());
	} else {
		clear(container);
		container = dynamic_array<T>(first, last);
	}
}

namespace detail {

template<typename T, typename Size, typename... MaybeInitializer>
auto resize(common_resize_tag, dynamic_array<T> & container, Size const count, MaybeInitializer && ... args) {
	static_assert(sizeof...(MaybeInitializer) == 0 or sizeof...(MaybeInitializer) == 1);
	if (size(container) != count) {
		// Technically, the clear is unnecessary, but calling it decreases peak
		// memory use.
		clear(container);
		container = dynamic_array<T>(count, std::forward<MaybeInitializer>(args)...);
	}
}

}	// namespace detail

}	// namespace containers
