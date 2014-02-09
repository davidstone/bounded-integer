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
#include "numeric_limits.hpp"

#include <iterator>

namespace bounded_integer {

template<typename Integer>
class immutable_range;

namespace detail {
namespace range_iterator {

template<typename integer>
constexpr intmax_t range_of_type() noexcept {
	return static_cast<intmax_t>(std::numeric_limits<integer>::max() - std::numeric_limits<integer>::min());
}

template<typename integer>
class iterator {
public:
	using value_type = integer;
	// We have to be able to index the one-past-the-end element. Not sure if
	// this should have a throw_policy.
	using index_type = bounded_integer<0, range_of_type<value_type>() + 1, null_policy>;
	using difference_type = bounded_integer<-range_of_type<index_type>(), range_of_type<index_type>(), null_policy>;
	using pointer = index_type const *;
	using iterator_category = std::random_access_iterator_tag;

	iterator() = default;
	
	// It is undefined behavior to dereference a past-the-end iterator
	constexpr value_type operator*() const noexcept {
		return value_type(m_index, non_check);
	}
	constexpr pointer operator->() const noexcept {
		return &m_index;
	}
	constexpr value_type operator[](index_type const & index) const {
		return static_cast<value_type>(m_index + index);
	}
	
	friend constexpr difference_type operator-(iterator const & lhs, iterator const & rhs) noexcept {
		return lhs.m_index - rhs.m_index;
	}
	friend constexpr iterator operator+(iterator const & lhs, difference_type const & rhs) {
		return iterator(static_cast<index_type>(lhs.m_index + rhs));
	}
	
	friend constexpr bool operator==(iterator const & lhs, iterator const & rhs) noexcept {
		return lhs.m_index == rhs.m_index;
	}
	friend constexpr bool operator<(iterator const & lhs, iterator const & rhs) noexcept {
		return lhs.m_index < rhs.m_index;
	}
private:
	friend class immutable_range<value_type>;
	using underlying_type = bounded_integer<
		static_cast<intmax_t>(std::numeric_limits<value_type>::min()),
		static_cast<intmax_t>(std::numeric_limits<value_type>::max() + make_bounded<1>()),
		null_policy
	>;
	explicit constexpr iterator(underlying_type const index) noexcept:
		m_index(index) {
	}
	underlying_type m_index;
};

template<typename integer>
constexpr iterator<integer> operator+(typename iterator<integer>::difference_type const & lhs, iterator<integer> const & rhs) {
	return rhs + lhs;
}
template<typename integer>
iterator<integer> & operator+=(iterator<integer> & lhs, typename iterator<integer>::difference_type const & rhs) {
	return lhs = lhs + rhs;
}
template<typename integer>
constexpr iterator<integer> operator-(iterator<integer> const & lhs, typename iterator<integer>::difference_type const & rhs) {
	return lhs + (-rhs);
}
template<typename integer>
iterator<integer> & operator-=(iterator<integer> & lhs, typename iterator<integer>::difference_type const & rhs) {
	return lhs = lhs - rhs;
}

template<typename integer>
iterator<integer> & operator++(iterator<integer> & it) {
	return it += make_bounded<1>();
}
template<typename integer>
iterator<integer> operator++(iterator<integer> & it, int) {
	auto self = it;
	++it;
	return self;
}

template<typename integer>
iterator<integer> & operator--(iterator<integer> & it) {
	return it -= make_bounded<1>();
}
template<typename integer>
iterator<integer> operator--(iterator<integer> & it, int) {
	auto self = it;
	--it;
	return self;
}


template<typename integer>
constexpr bool operator!=(iterator<integer> const & lhs, iterator<integer> const & rhs) noexcept {
	return !(lhs == rhs);
}

template<typename integer>
constexpr bool operator>(iterator<integer> const & lhs, iterator<integer> const & rhs) noexcept {
	return rhs < lhs;
}
template<typename integer>
constexpr bool operator>=(iterator<integer> const & lhs, iterator<integer> const & rhs) noexcept {
	return !(lhs < rhs);
}
template<typename integer>
constexpr bool operator<=(iterator<integer> const & lhs, iterator<integer> const & rhs) noexcept {
	return !(rhs < lhs);
}

}	// namespace range_iterator
}	// namespace detail

template<typename Integer>
class immutable_range {
public:
	using value_type = Integer;
	static_assert(std::numeric_limits<value_type>::is_specialized, "Must be a numeric type.");

	using const_iterator = detail::range_iterator::iterator<Integer>;
	using const_reverse_iterator = std::reverse_iterator<const_iterator>;
	using const_pointer = typename const_iterator::pointer;

	using size_type = bounded_integer<0, detail::range_iterator::range_of_type<value_type>() + 1, null_policy>;
	using difference_type = typename const_iterator::difference_type;
	using index_type = typename const_iterator::index_type;

	template<typename Last>
	constexpr immutable_range(value_type const & first, Last const & last) noexcept:
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
	template<typename integer>
	constexpr value_type at(integer const & index) const {
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

template<typename Size>
constexpr auto range(Size && size) noexcept {
	using range_type = bounded_integer<0, static_cast<intmax_t>(std::numeric_limits<Size>::max()) - 1, null_policy>;
	return immutable_range<range_type>(make_bounded<0>(), std::forward<Size>(size));
}

template<typename Begin, typename End>
constexpr auto range(Begin && begin, End && end) noexcept {
	using range_type = bounded_integer<
		static_cast<intmax_t>(std::numeric_limits<Begin>::min()),
		static_cast<intmax_t>(std::numeric_limits<End>::max()) - 1,
		null_policy
	>;
	return immutable_range<range_type>(std::forward<Begin>(begin), std::forward<End>(end));
}

}	// namespace bounded_integer

#endif	// BOUNDED_INTEGER_RANGE_HPP_
