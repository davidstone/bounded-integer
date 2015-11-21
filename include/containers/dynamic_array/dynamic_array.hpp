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

#include <containers/algorithms/copy.hpp>
#include <containers/array/iterator.hpp>

#include <containers/uninitialized_storage.hpp>
#include <containers/common_container_functions.hpp>
#include <containers/repeat_n.hpp>

#include <iterator>
#include <memory>

namespace containers {

// TODO: Support stateful allocators in implementation
template<typename T, typename Allocator = std::allocator<T>>
struct dynamic_array {
	using value_type = T;
	using size_type = bounded::integer<0, std::numeric_limits<std::intmax_t>::max() / sizeof(T)>;

	using allocator_type = typename std::allocator_traits<Allocator>::template rebind_alloc<value_type>;
	static_assert(std::is_empty<allocator_type>::value, "Stateful allocators not yet supported.");

	using const_iterator = detail::basic_array_iterator<value_type const, dynamic_array>;
	using iterator = detail::basic_array_iterator<value_type, dynamic_array>;
	
	static constexpr auto get_allocator() BOUNDED_NOEXCEPT(
		allocator_type{}
	)
	
	constexpr dynamic_array() = default;

	constexpr dynamic_array(allocator_type) {
	}
	
	template<typename Count, BOUNDED_REQUIRES(std::is_convertible<Count, size_type>::value)>
	explicit dynamic_array(Count const count, allocator_type = allocator_type()):
		m_size(count),
		m_data(make_storage(m_size))
	{
		detail::uninitialized_default_construct(begin(), end(), get_allocator());
	}
	
	template<typename ForwardIterator, typename Sentinel>
	dynamic_array(ForwardIterator first, Sentinel const last, allocator_type = allocator_type()):
		m_size(detail::distance(first, last)),
		m_data(make_storage(m_size))
	{
		detail::uninitialized_copy(first, last, m_data.get(), get_allocator());
	}
	dynamic_array(std::initializer_list<value_type> init, allocator_type = allocator_type()):
		dynamic_array(init.begin(), init.end())
	{
	}
	template<typename Count, BOUNDED_REQUIRES(std::is_convertible<Count, size_type>::value)>
	dynamic_array(Count const count, value_type const & value, allocator_type = allocator_type()) {
		auto const range = ::containers::detail::repeat_n(count, value);
		*this = dynamic_array(range.begin(), range.end());
	}
	
	dynamic_array(dynamic_array const & other, allocator_type = allocator_type()):
		dynamic_array(other.begin(), other.end())
	{
	}
	constexpr dynamic_array(dynamic_array && other, allocator_type = allocator_type()) noexcept:
		m_size(std::move(other.m_size)),
		m_data(std::move(other.m_data))
	{
		other.m_size = bounded::constant<0>;
	}

	auto & operator=(dynamic_array const & other) & {
		assign(*this, other.begin(), other.end());
		return *this;
	}
	constexpr auto & operator=(dynamic_array && other) & noexcept {
		m_size = std::move(other.m_size);
		m_data = std::move(other.m_data);
		other.m_size = bounded::constant<0>;
		return *this;
	}
	
	constexpr auto data() const noexcept {
		return reinterpret_cast<value_type const *>(m_data.get());
	}
	constexpr auto data() noexcept {
		return reinterpret_cast<value_type *>(m_data.get());
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
	using underlying_storage = uninitialized_storage<value_type>[];
	static auto make_storage(size_type const size) {
		return (size == bounded::constant<0>) ? nullptr : std::make_unique<underlying_storage>(static_cast<std::size_t>(size));
	}
	size_type m_size = bounded::constant<0>;
	std::unique_ptr<underlying_storage> m_data = nullptr;
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
