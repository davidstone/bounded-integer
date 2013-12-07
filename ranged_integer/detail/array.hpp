// Array class with better interoperability with ranged_integer than std::array
// Copyright (C) 2013 David Stone
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

#ifndef RANGED_INTEGER_ARRAY_HPP_
#define RANGED_INTEGER_ARRAY_HPP_

#include "class.hpp"
#include "make_ranged.hpp"
#include "policy/null_policy.hpp"
#include "policy/throw_policy.hpp"

#include <algorithm>
#include <cstdint>
#include <iterator>
#include <limits>
#include <utility>

namespace detail {

template<typename T, intmax_t size>
class iterator {
private:
	static constexpr intmax_t size_range = (size == 0) ? 0 : (size - 1);
public:
	using value_type = T;
	using difference_type = ranged_integer<-size_range, size_range, null_policy>;
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
		return lhs.m_it - rhs.m_it;
	}
	constexpr reference operator[](ranged_integer<0, size - 1, null_policy> const index) const {
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
//	using size_type = ?;
	using difference_type = typename detail::iterator<value_type, size_>::difference_type;
	using const_reference = value_type const &;
	using reference = value_type &;
	using const_pointer = value_type const *;
	using pointer = value_type *;
	using const_iterator = detail::iterator<value_type const, size_>;
	using iterator = detail::iterator<value_type, size_>;
	using const_reverse_iterator = std::reverse_iterator<const_iterator>;
	using reverse_iterator = std::reverse_iterator<iterator>;
	
	template<typename... Args>
	constexpr array(Args && ... args):
		m_value{ std::forward<Args>(args)... } {
	}

	constexpr auto size() const noexcept -> decltype(make_ranged<size_>()) {
		return make_ranged<size_>();
	}
	constexpr bool empty() const noexcept {
		return size() == 0;
	}
	constexpr auto max_size() const noexcept -> decltype(make_ranged<std::numeric_limits<std::size_t>::max() / sizeof(value_type)>()) {
		return make_ranged<std::numeric_limits<std::size_t>::max() / sizeof(value_type)>();
	}
	
	
	template<typename index_type>
	constexpr const_reference at(index_type const & index) const {
		return m_value[static_cast<std::size_t>(ranged_integer<0, size_ - 1, throw_policy>(index))];
	}
	template<typename index_type>
	reference at(index_type const & index) {
		return m_value[static_cast<std::size_t>(ranged_integer<0, size_ - 1, throw_policy>(index))];
	}
	constexpr const_reference operator[](ranged_integer<0, size_ - 1, null_policy> const & index) const noexcept {
		return m_value[static_cast<std::size_t>(index)];
	}
	reference operator[](ranged_integer<0, size_ - 1, null_policy> const & index) noexcept {
		return m_value[static_cast<std::size_t>(index)];
	}

	constexpr const_reference front() const {
		return m_value[0];
	}
	reference front() {
		return m_value[0];
	}
	constexpr const_reference back() const {
		return m_value[size() - 1];
	}
	reference back() {
		return m_value[size() - 1];
	}

	constexpr const_pointer data() const noexcept {
		return m_value;
	}
	pointer data() noexcept {
		return m_value;
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
		return const_iterator(m_value + size());
	}
	iterator end() noexcept {
		return iterator(m_value + size());
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
private:
	using array_type = value_type[size_];
	array_type m_value;
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
	return a[make_ranged<index>()];
}
template<std::size_t index, typename T, std::size_t size>
constexpr T & get(array<T, size> & a) noexcept {
	return a[make_ranged<index>()];
}
template<std::size_t index, typename T, std::size_t size>
constexpr T && get(array<T, size> && a) noexcept {
	return std::move(a[make_ranged<index>()]);
}

template<typename T, std::size_t size>
void swap(array<T, size> & lhs, array<T, size> & rhs) noexcept(noexcept(lhs.swap(rhs))) {
	lhs.swap(rhs);
}

namespace std {

template<typename T, size_t size>
class tuple_size<::array<T, size>> : public integral_constant<size_t, size> {};

template<size_t index, typename T, size_t size>
struct tuple_element<index, ::array<T, size>> {
	using type = T;
};

}	// namespace std

#endif	// RANGED_INTEGER_ARRAY_HPP_
