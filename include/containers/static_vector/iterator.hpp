// std::vector-like interface around a bounded::array
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

#include <containers/static_vector/forward_declaration.hpp>

#include <bounded_integer/array.hpp>

#include <cstddef>
#include <type_traits>

namespace containers {
namespace detail {

template<typename T, std::size_t capacity>
struct static_vector_iterator {
private:
	friend struct static_vector<std::remove_const_t<T>, capacity>;
	
	using container = bounded::array<uninitialized_storage<std::remove_const_t<T>>, capacity>;
	using base_iterator = std::conditional_t<std::is_const<T>::value, typename container::const_iterator, typename container::iterator>;

public:
	using iterator_category = typename base_iterator::iterator_category;
	using value_type = T;
	using difference_type = typename base_iterator::difference_type;
	using pointer = T *;
	using reference = T &;

	constexpr auto && operator*() const {
		return reinterpret_cast<value_type &>(*m_it);
	}
	constexpr auto operator->() const {
		return std::addressof(operator*());
	}
	
	template<typename Offset, BOUNDED_REQUIRES(std::numeric_limits<Offset>::is_integer)>
	friend constexpr auto operator+(static_vector_iterator const it, Offset const offset) {
		return static_vector_iterator(it.m_it + offset);
	}
	friend constexpr auto operator-(static_vector_iterator const lhs, static_vector_iterator const rhs) {
		return lhs.m_it - rhs.m_it;
	}

	friend constexpr auto operator==(static_vector_iterator const lhs, static_vector_iterator const rhs) {
		return lhs.m_it == rhs.m_it;
	}
	friend constexpr auto operator<(static_vector_iterator const lhs, static_vector_iterator const rhs) {
		return lhs.m_it < rhs.m_it;
	}
	
	// Convert iterator to const_iterator
	constexpr operator static_vector_iterator<T const, capacity>() const {
		return static_vector_iterator<T const, capacity>(m_it);
	}
	
private:
	friend struct static_vector_iterator<std::remove_const_t<T>, capacity>;

	constexpr explicit static_vector_iterator(base_iterator const it):
		m_it(it) {
	}

	base_iterator m_it;
};

template<typename T, std::size_t capacity, typename Offset, BOUNDED_REQUIRES(std::numeric_limits<Offset>::is_integer)>
constexpr auto operator-(static_vector_iterator<T, capacity> const it, Offset const offset) {
	return it + -offset;
}

template<typename T, std::size_t capacity, typename Offset, BOUNDED_REQUIRES(std::numeric_limits<Offset>::is_integer)>
constexpr auto & operator+=(static_vector_iterator<T, capacity> & it, Offset const offset) {
	return it = it + offset;
}

template<typename T, std::size_t capacity, typename Offset, BOUNDED_REQUIRES(std::numeric_limits<Offset>::is_integer)>
constexpr auto & operator-=(static_vector_iterator<T, capacity> & it, Offset const offset) {
	return it = it - offset;
}

template<typename T, std::size_t capacity>
constexpr auto & operator++(static_vector_iterator<T, capacity> & it) {
	return it += bounded::constant<1>;
}

template<typename T, std::size_t capacity>
constexpr auto operator++(static_vector_iterator<T, capacity> & it, int) {
	auto temp = it;
	++it;
	return temp;
}



template<typename T, std::size_t capacity>
constexpr auto operator!=(static_vector_iterator<T, capacity> const lhs, static_vector_iterator<T, capacity> const rhs) {
	return !(lhs == rhs);
}

template<typename T, std::size_t capacity>
constexpr auto operator>(static_vector_iterator<T, capacity> const lhs, static_vector_iterator<T, capacity> const rhs) noexcept {
	return rhs < lhs;
}
template<typename T, std::size_t capacity>
constexpr auto operator<=(static_vector_iterator<T, capacity> const lhs, static_vector_iterator<T, capacity> const rhs) noexcept {
	return !(rhs < lhs);
}
template<typename T, std::size_t capacity>
constexpr auto operator>=(static_vector_iterator<T, capacity> const lhs, static_vector_iterator<T, capacity> const rhs) noexcept {
	return !(lhs < rhs);
}

}	// namespace detail
}	// namespace containers
