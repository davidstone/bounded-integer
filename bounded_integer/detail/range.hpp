// Iterate over a numeric range
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

#ifndef BOUNDED_INTEGER_RANGE_HPP_
#define BOUNDED_INTEGER_RANGE_HPP_

#include "arithmetic_operators.hpp"
#include "class.hpp"
#include "comparison_operators.hpp"
#include "make_bounded.hpp"
#include "minmax.hpp"
#include "numeric_limits.hpp"

#include <iterator>

namespace bounded_integer {

template<typename T>
class immutable_range;

namespace detail {
namespace range_iterator {

template<typename T>
constexpr intmax_t range_of_type() noexcept {
	return static_cast<intmax_t>(std::numeric_limits<T>::max() - std::numeric_limits<T>::min());
}

template<typename T>
class iterator {
public:
	// We have to be able to index the one-past-the-end element. Not sure if
	// this should have a throw_policy.
	using index_type = bounded_integer<0, range_of_type<T>(), null_policy>;
	using value_type = bounded_integer<
		static_cast<intmax_t>(std::numeric_limits<T>::min()),
		static_cast<intmax_t>(max(std::numeric_limits<T>::min(), std::numeric_limits<T>::max() - make_bounded<1>())),
		null_policy
	>;
	using difference_type = bounded_integer<-range_of_type<index_type>(), range_of_type<index_type>(), null_policy>;
	using pointer = index_type const *;
	using iterator_category = std::random_access_iterator_tag;

	iterator() = default;
	
	// It is undefined behavior to dereference a past-the-end iterator
	constexpr value_type operator*() const noexcept {
		return value_type(m_value, non_check);
	}
	constexpr pointer operator->() const noexcept {
		return &m_value;
	}
	constexpr value_type operator[](index_type const & index) const {
		return static_cast<value_type>(m_value + index);
	}
	
	friend constexpr difference_type operator-(iterator const & lhs, iterator const & rhs) noexcept {
		return lhs.m_value - rhs.m_value;
	}
	friend constexpr iterator operator+(iterator const & lhs, difference_type const & rhs) {
		return iterator(static_cast<underlying_type>(lhs.m_value + rhs));
	}
	
	friend constexpr bool operator==(iterator const & lhs, iterator const & rhs) noexcept {
		return lhs.m_value == rhs.m_value;
	}
	friend constexpr bool operator<(iterator const & lhs, iterator const & rhs) noexcept {
		return lhs.m_value < rhs.m_value;
	}
private:
	friend class immutable_range<T>;
	using underlying_type = T;
	explicit constexpr iterator(underlying_type const value) noexcept:
		m_value(value) {
	}
	underlying_type m_value;
};

template<typename T>
constexpr iterator<T> operator+(typename iterator<T>::difference_type const & lhs, iterator<T> const & rhs) {
	return rhs + lhs;
}
template<typename T>
iterator<T> & operator+=(iterator<T> & lhs, typename iterator<T>::difference_type const & rhs) {
	return lhs = lhs + rhs;
}
template<typename T>
constexpr iterator<T> operator-(iterator<T> const & lhs, typename iterator<T>::difference_type const & rhs) {
	return lhs + (-rhs);
}
template<typename T>
iterator<T> & operator-=(iterator<T> & lhs, typename iterator<T>::difference_type const & rhs) {
	return lhs = lhs - rhs;
}

template<typename T>
iterator<T> & operator++(iterator<T> & it) {
	return it += make_bounded<1>();
}
template<typename T>
iterator<T> operator++(iterator<T> & it, int) {
	auto self = it;
	++it;
	return self;
}

template<typename T>
iterator<T> & operator--(iterator<T> & it) {
	return it -= make_bounded<1>();
}
template<typename T>
iterator<T> operator--(iterator<T> & it, int) {
	auto self = it;
	--it;
	return self;
}


template<typename T>
constexpr bool operator!=(iterator<T> const & lhs, iterator<T> const & rhs) noexcept {
	return !(lhs == rhs);
}

template<typename T>
constexpr bool operator>(iterator<T> const & lhs, iterator<T> const & rhs) noexcept {
	return rhs < lhs;
}
template<typename T>
constexpr bool operator>=(iterator<T> const & lhs, iterator<T> const & rhs) noexcept {
	return !(lhs < rhs);
}
template<typename T>
constexpr bool operator<=(iterator<T> const & lhs, iterator<T> const & rhs) noexcept {
	return !(rhs < lhs);
}

}	// namespace range_iterator
}	// namespace detail

template<typename T>
class immutable_range {
public:
	static_assert(std::numeric_limits<T>::is_specialized, "Must be a numeric type.");
	using const_iterator = detail::range_iterator::iterator<T>;
	using const_reverse_iterator = std::reverse_iterator<const_iterator>;

	using value_type = typename const_iterator::value_type;
	using index_type = typename const_iterator::index_type;
	using difference_type = typename const_iterator::difference_type;
	using const_pointer = typename const_iterator::pointer;
	using size_type = bounded_integer<0, detail::range_iterator::range_of_type<value_type>(), null_policy>;
	// This accounts for the one-past-the-end sentinel value.
	using underlying_type = typename const_iterator::underlying_type;

	constexpr immutable_range(underlying_type const & first, underlying_type const & last) noexcept:
		m_begin(first),
		m_end(last) {
	}

	constexpr const_iterator begin() const noexcept {
		return m_begin;
	}
	constexpr const_iterator cbegin() const noexcept {
		return begin();
	}
	constexpr const_iterator end() const noexcept {
		return m_end;
	}
	constexpr const_iterator cend() const {
		return end();
	}
	constexpr const_reverse_iterator rbegin() const noexcept {
		return const_reverse_iterator(end());
	}
	constexpr const_reverse_iterator crbegin() const noexcept {
		return rbegin();
	}
	constexpr const_reverse_iterator rend() const noexcept {
		return const_reverse_iterator(begin());
	}
	constexpr const_reverse_iterator crend() const noexcept {
		return rend();
	}

	constexpr value_type front() const {
		return *begin();
	}
	constexpr value_type back() const {
		return *(end() - make_bounded<1>());
	}
	constexpr value_type operator[](index_type const & index) const {
		return begin()[index];
	}
	template<typename Index>
	constexpr value_type at(Index const & index) const {
		return begin()[static_cast<index_type>(index)];
	}

	void swap(immutable_range & other) noexcept {
		using std::swap;
		swap(m_begin, other.m_begin);
		swap(m_end, other.m_end);
	}
	constexpr size_type size() const {
		return size_type(end() - begin(), non_check);
	}
	constexpr size_type max_size() const {
		return make_bounded<detail::range_iterator::range_of_type<value_type>()>();
	}
	constexpr bool empty() const {
		return size() == make_bounded<0>();
	}

private:
	const_iterator m_begin;
	const_iterator m_end;
};

template<typename T>
void swap(immutable_range<T>& lhs, immutable_range<T>& rhs) {
	lhs.swap(rhs);
}

template<typename Begin, typename End>
constexpr auto range(Begin && begin, End && end) noexcept {
	using range_type = bounded_integer<
		static_cast<intmax_t>(std::numeric_limits<typename std::decay<Begin>::type>::min()),
		static_cast<intmax_t>(std::numeric_limits<typename std::decay<End>::type>::max()),
		null_policy
	>;
	return immutable_range<range_type>(std::forward<Begin>(begin), std::forward<End>(end));
}

template<typename Size>
constexpr auto range(Size && size) noexcept {
	return range(make_bounded<0>(), std::forward<Size>(size));
}

}	// namespace bounded_integer

#endif	// BOUNDED_INTEGER_RANGE_HPP_
