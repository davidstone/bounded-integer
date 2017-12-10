// A vector-like class that can take advantage of cheap moves
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

#include <containers/moving_vector/forward_declaration.hpp>

#include <bounded_integer/bounded_integer.hpp>

#include <value_ptr/value_ptr.hpp>

#include <iterator>
#include <type_traits>
#include <vector>

namespace containers {

namespace detail {
namespace moving_vector {
using namespace smart_pointer;

template<typename T, typename ValueType, BOUNDED_REQUIRES(std::is_same<T, ValueType>::value)>
constexpr decltype(auto) remove_indirection(value_ptr<T> const * ptr) {
	return **ptr;
}
template<typename T, typename ValueType, BOUNDED_REQUIRES(std::is_same<T, ValueType>::value)>
constexpr decltype(auto) remove_indirection(value_ptr<T> * ptr) {
	return **ptr;
}

template<typename T, typename ValueType, BOUNDED_REQUIRES(std::is_same<value_ptr<T>, ValueType>::value)>
constexpr decltype(auto) remove_indirection(value_ptr<T> const * ptr) {
	return *ptr;
}
template<typename T, typename ValueType, BOUNDED_REQUIRES(std::is_same<value_ptr<T>, ValueType>::value)>
constexpr decltype(auto) remove_indirection(value_ptr<T> * ptr) {
	return *ptr;
}

template<typename T, typename ValueType>
class iterator_base {
private:
	using indirection_type = value_ptr<T>;
public:
	using value_type = ValueType;
	using difference_type = std::ptrdiff_t;
	using pointer = value_type *;
	using reference = value_type &;
	using iterator_category = std::random_access_iterator_tag;

	constexpr iterator_base() noexcept = default;
	// Convert iterator or indirect_iterator to const_iterator
	constexpr operator iterator_base<T, T const>() const noexcept {
		return iterator_base<T, T const>(it);
	}
	// Convert iterator to indirect_iterator.
	constexpr explicit operator iterator_base<T, indirection_type>() const noexcept {
		return iterator_base<T, indirection_type>(it);
	}

	reference operator*() const {
		return remove_indirection<std::remove_const_t<T>, std::remove_const_t<value_type>>(it);
	}
	pointer operator->() const {
		return & this->operator*();
	}
	iterator_base & operator++() {
		++it;
		return *this;
	}
	iterator_base operator++(int) {
		auto const self = *this;
		operator++();
		return self;
	}
	iterator_base & operator--() {
		--it;
		return *this;
	}
	iterator_base operator--(int) {
		auto const self = *this;
		operator--();
		return self;
	}
	iterator_base & operator+=(difference_type const offset) {
		it += offset;
		return *this;
	}
	iterator_base & operator-=(difference_type const offset) {
		it -= offset;
		return *this;
	}
	friend constexpr difference_type operator-(iterator_base const & lhs, iterator_base const & rhs) {
		return lhs.it - rhs.it;
	}
	template<typename Integer>
	reference operator[](Integer const index) {
		return it[index];
	}

	friend constexpr bool operator==(iterator_base const & lhs, iterator_base const & rhs) noexcept {
		return lhs.it == rhs.it;
	}
	friend constexpr bool operator<(iterator_base const & lhs, iterator_base const & rhs) noexcept {
		return lhs.it < rhs.it;
	}
	
private:
	template<typename U, typename Allocator>
	friend class ::containers::moving_vector;
	template<typename U, typename VT>
	friend class iterator_base;

	static constexpr bool is_const = std::is_const<value_type>::value;
	using base_iterator = std::conditional_t<is_const, indirection_type const, indirection_type> *;

	constexpr explicit iterator_base(base_iterator const other) noexcept:
		it(other) {
	}
	constexpr explicit iterator_base(typename std::vector<indirection_type>::iterator const other):
		it(&*other) {
	}
	constexpr explicit iterator_base(typename std::vector<indirection_type>::const_iterator const other):
		it(&*other) {
	}

	// Convert const_iterator to iterator
	constexpr explicit operator iterator_base<T, T>() const noexcept {
		using mutable_type = std::conditional_t<std::is_const<T>::value, indirection_type const, indirection_type> *;
		return iterator_base<T, T>(const_cast<mutable_type>(it));
	}

	base_iterator it;
};

template<typename T, typename ValueType>
constexpr bool operator!=(iterator_base<T, ValueType> const lhs, iterator_base<T, ValueType> const rhs) noexcept {
	return !(lhs == rhs);
}
template<typename T, typename ValueType>
constexpr bool operator>(iterator_base<T, ValueType> const lhs, iterator_base<T, ValueType> const rhs) noexcept {
	return rhs < lhs;
}
template<typename T, typename ValueType>
constexpr bool operator<=(iterator_base<T, ValueType> const lhs, iterator_base<T, ValueType> const rhs) noexcept {
	return !(lhs > rhs);
}
template<typename T, typename ValueType>
constexpr bool operator>=(iterator_base<T, ValueType> const lhs, iterator_base<T, ValueType> const rhs) noexcept {
	return !(lhs < rhs);
}

template<typename T, typename ValueType>
constexpr iterator_base<T, ValueType> operator+(iterator_base<T, ValueType> lhs, typename iterator_base<T, ValueType>::difference_type const rhs) {
	return lhs += rhs;
}
template<typename T, typename ValueType>
constexpr iterator_base<T, ValueType> operator+(typename iterator_base<T, ValueType>::difference_type const lhs, iterator_base<T, ValueType> rhs) {
	return rhs += lhs;
}
template<typename T, typename ValueType>
constexpr iterator_base<T, ValueType> operator-(iterator_base<T, ValueType> lhs, typename iterator_base<T, ValueType>::difference_type const rhs) {
	return lhs -= rhs;
}

}	// namespace moving_vector
}	// namespace detail
}	// namespace containers
