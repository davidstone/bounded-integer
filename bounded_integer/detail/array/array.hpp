// Array class with better interoperability with bounded_integer than std::array
// Copyright (C) 2014 David Stone
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

#ifndef BOUNDED_INTEGER_DETAIL_ARRAY_HPP_
#define BOUNDED_INTEGER_DETAIL_ARRAY_HPP_

#include "../class.hpp"
#include "../make_bounded.hpp"
#include "../policy/null_policy.hpp"
#include "../policy/throw_policy.hpp"

#include <algorithm>
#include <cstdint>
#include <iterator>
#include <limits>
#include <utility>

namespace bounded_integer {
namespace detail {

template<typename T, intmax_t size>
class iterator {
public:
	using value_type = T;
	using difference_type = bounded_integer<-size, size, null_policy>;
	using index_type = bounded_integer<0, size - 1, throw_policy>;
	using pointer = value_type *;
	using reference = value_type &;
	using iterator_category = std::random_access_iterator_tag;

	constexpr iterator() noexcept = default;
	// Convert iterator to const_iterator
	constexpr operator iterator<T const, size>() const noexcept {
		return iterator<T const, size>(m_it);
	}

	constexpr reference operator*() const {
		return *m_it;
	}
	constexpr pointer operator->() const {
		return & operator*();
	}
	iterator & operator++() {
		++m_it;
		return *this;
	}
	iterator operator++(int) {
		auto const self = *this;
		operator++();
		return self;
	}
	iterator & operator--() {
		--m_it;
		return *this;
	}
	iterator operator--(int) {
		auto const self = *this;
		operator--();
		return self;
	}
	iterator & operator+=(difference_type const offset) {
		m_it += offset;
		return *this;
	}
	iterator & operator-=(difference_type const offset) {
		m_it -= offset;
		return *this;
	}
	friend constexpr difference_type operator-(iterator const & lhs, iterator const & rhs) {
		return static_cast<difference_type>(lhs.m_it - rhs.m_it);
	}
	constexpr reference operator[](index_type const index) const {
		return m_it[static_cast<std::size_t>(index)];
	}

	friend constexpr bool operator==(iterator const & lhs, iterator const & rhs) noexcept {
		return lhs.m_it == rhs.m_it;
	}
	friend constexpr bool operator<(iterator const & lhs, iterator const & rhs) noexcept {
		return lhs.m_it < rhs.m_it;
	}

private:
	template<typename U, std::size_t size_>
	friend class array;
	template<typename U, intmax_t size_>
	friend class iterator;

	using base_iterator = value_type *;
	constexpr explicit iterator(base_iterator const other) noexcept:
		m_it(other) {
	}

	base_iterator m_it;
};

template<typename T, intmax_t size>
constexpr bool operator!=(iterator<T, size> const lhs, iterator<T, size> const rhs) noexcept {
	return !(lhs == rhs);
}
template<typename T, intmax_t size>
constexpr bool operator>(iterator<T, size> const lhs, iterator<T, size> const rhs) noexcept {
	return rhs < lhs;
}
template<typename T, intmax_t size>
constexpr bool operator<=(iterator<T, size> const lhs, iterator<T, size> const rhs) noexcept {
	return !(lhs > rhs);
}
template<typename T, intmax_t size>
constexpr bool operator>=(iterator<T, size> const lhs, iterator<T, size> const rhs) noexcept {
	return !(lhs < rhs);
}

template<typename T, intmax_t size>
constexpr iterator<T, size> operator+(iterator<T, size> lhs, typename iterator<T, size>::difference_type const rhs) {
	return lhs += rhs;
}
template<typename T, intmax_t size>
constexpr iterator<T, size> operator+(typename iterator<T, size>::difference_type const lhs, iterator<T, size> rhs) {
	return rhs += lhs;
}
template<typename T, intmax_t size>
constexpr iterator<T, size> operator-(typename iterator<T, size>::difference_type const lhs, iterator<T, size> rhs) {
	return lhs -= rhs;
}

namespace adl {

using std::swap;

template<typename T, typename U>
struct is_nothrow_swappable : std::integral_constant<bool, noexcept(swap(std::declval<T &>(), std::declval<U &>()))> {};

}	//	namespace adl
}	// namespace detail

template<typename T, std::size_t size_>
class array {
public:
	using value_type = T;

	using const_iterator = detail::iterator<value_type const, size_>;
	using iterator = detail::iterator<value_type, size_>;
	using const_reverse_iterator = std::reverse_iterator<const_iterator>;
	using reverse_iterator = std::reverse_iterator<iterator>;

	using size_type = bounded_integer<size_, size_, null_policy>;
	using difference_type = typename const_iterator::difference_type;
	using index_type = typename const_iterator::index_type;
	using const_reference = typename const_iterator::reference;
	using reference = typename iterator::reference;
	using const_pointer = typename const_iterator::pointer;
	using pointer = typename iterator::pointer;
	
	constexpr size_type size() const noexcept {
		return make_bounded<size_>();
	}
	constexpr bool empty() const noexcept {
		return size() == make_bounded<0>();
	}
	constexpr size_type max_size() const noexcept {
		return size();
	}
	
	
	constexpr const_pointer data() const noexcept {
		return m_value;
	}
	pointer data() noexcept {
		return m_value;
	}

	constexpr const_reference operator[](index_type const & index) const noexcept {
		return m_value[static_cast<std::size_t>(index)];
	}
	reference operator[](index_type const & index) noexcept {
		return m_value[static_cast<std::size_t>(index)];
	}
	template<typename integer>
	constexpr const_reference at(integer const & index) const noexcept(noexcept(static_cast<index_type>(index))) {
		return m_value[static_cast<std::size_t>(static_cast<index_type>(index))];
	}
	template<typename integer>
	reference at(integer const & index) noexcept(noexcept(static_cast<index_type>(index))) {
		return m_value[static_cast<std::size_t>(static_cast<index_type>(index))];
	}

	constexpr const_reference front() const noexcept {
		return m_value[0];
	}
	reference front() noexcept {
		return m_value[0];
	}
	constexpr const_reference back() const noexcept {
		return m_value[size_ - 1];
	}
	reference back() noexcept {
		return m_value[size_ - 1];
	}

	constexpr const_iterator begin() const noexcept {
		return const_iterator(m_value);
	}
	iterator begin() noexcept {
		return iterator(m_value);
	}
	constexpr const_iterator cbegin() const noexcept {
		return begin();
	}

	constexpr const_iterator end() const noexcept {
		return const_iterator(m_value + size_);
	}
	iterator end() noexcept {
		return iterator(m_value + size_);
	}
	constexpr const_iterator cend() const noexcept {
		return end();
	}

	constexpr const_reverse_iterator rbegin() const noexcept {
		return const_reverse_iterator(end());
	}
	reverse_iterator rbegin() noexcept {
		return reverse_iterator(end());
	}
	constexpr const_reverse_iterator crbegin() const noexcept {
		return rbegin();
	}

	constexpr const_reverse_iterator rend() const noexcept {
		return const_reverse_iterator(begin());
	}
	reverse_iterator rend() noexcept {
		return reverse_iterator(begin());
	}
	constexpr const_reverse_iterator crend() const noexcept {
		return rend();
	}
	
	void fill(const_reference value) noexcept(std::is_nothrow_copy_constructible<value_type>::value) {
		std::fill(begin(), end(), value);
	}
	
	void swap(array & other) noexcept(detail::adl::is_nothrow_swappable<reference, reference>::value) {
		std::swap_ranges(begin(), end(), other.begin());
	}

	// Consider this private. It must be public to be an aggregate
	value_type m_value[size_];
};

template<typename T, std::size_t size>
constexpr bool operator==(array<T, size> const & lhs, array<T, size> const & rhs) noexcept(noexcept(std::declval<typename array<T, size>::value_type>() == std::declval<typename array<T, size>::value_type>())) {
	return std::equal(lhs.begin(), lhs.end(), rhs.begin());
}

template<typename T, std::size_t size>
constexpr bool operator!=(array<T, size> const & lhs, array<T, size> const & rhs) noexcept(noexcept(!(lhs == rhs))) {
	return !(lhs == rhs);
}

template<typename T, std::size_t size>
constexpr bool operator<(array<T, size> const & lhs, array<T, size> const & rhs) noexcept(noexcept(std::declval<typename array<T, size>::value_type>() < std::declval<typename array<T, size>::value_type>())) {
	return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

template<typename T, std::size_t size>
constexpr bool operator>(array<T, size> const & lhs, array<T, size> const & rhs) noexcept(noexcept(rhs < lhs)) {
	return rhs < lhs;
}

template<typename T, std::size_t size>
constexpr bool operator<=(array<T, size> const & lhs, array<T, size> const & rhs) noexcept(noexcept(!(rhs < lhs))) {
	return !(rhs < lhs);
}

template<typename T, std::size_t size>
constexpr bool operator>=(array<T, size> const & lhs, array<T, size> const & rhs) noexcept(noexcept(!(lhs < rhs))) {
	return !(lhs < rhs);
}

template<std::size_t index, typename T, std::size_t size>
constexpr T const & get(array<T, size> const & a) noexcept {
	return a[make_bounded<index>()];
}
template<std::size_t index, typename T, std::size_t size>
constexpr T & get(array<T, size> & a) noexcept {
	return a[make_bounded<index>()];
}
template<std::size_t index, typename T, std::size_t size>
constexpr T && get(array<T, size> && a) noexcept {
	return std::move(a[make_bounded<index>()]);
}

template<typename T, std::size_t size>
void swap(array<T, size> & lhs, array<T, size> & rhs) noexcept(noexcept(lhs.swap(rhs))) {
	lhs.swap(rhs);
}
}	// namespace bounded_integer

// I am not sure yet if it is legal to specialize these classes.
#if 0
namespace std {

template<typename T, size_t size>
struct tuple_size<::array<T, size>> : public integral_constant<size_t, size> {};

template<size_t index, typename T, size_t size>
struct tuple_element<index, ::array<T, size>> {
	using type = T;
};

}	// namespace std
#endif

#endif	// BOUNDED_INTEGER_DETAIL_ARRAY_HPP_
