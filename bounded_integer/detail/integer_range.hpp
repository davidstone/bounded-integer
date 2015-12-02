// Iterate over a numeric range
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

#include "class.hpp"
#include "comparison.hpp"
#include "minmax.hpp"
#include "numeric_limits.hpp"

#include "arithmetic/common.hpp"
#include "arithmetic/minus.hpp"
#include "arithmetic/plus.hpp"
#include "arithmetic/unary_minus.hpp"

#include "policy/throw_policy.hpp"

#include <iterator>

namespace bounded {

template<typename T>
struct range_type;

namespace detail {

template<typename T>
constexpr auto range_of_type() noexcept {
	return static_cast<intmax_t>(std::numeric_limits<T>::max() - std::numeric_limits<T>::min());
}

template<typename T>
struct integer_range_iterator {
	// We have to be able to index the one-past-the-end element.
	using index_type = integer<0, range_of_type<T>()>;
	using value_type = integer<
		static_cast<intmax_t>(std::numeric_limits<T>::min()),
		static_cast<intmax_t>(max(std::numeric_limits<T>::min(), std::numeric_limits<T>::max() - constant<1>))
	>;
	using difference_type = integer<-range_of_type<index_type>(), range_of_type<index_type>()>;
	using pointer = index_type const *;
	using iterator_category = std::random_access_iterator_tag;
	using underlying_type = T;

	integer_range_iterator() = default;
	explicit constexpr integer_range_iterator(underlying_type const value) noexcept:
		m_value(value) {
	}
	
	// It is undefined behavior to dereference a past-the-end iterator.
	//
	// Note that this returns a value, not a reference.
	constexpr auto operator*() const noexcept -> value_type {
		return value_type(m_value, non_check);
	}
	constexpr auto operator[](index_type const & index) const -> value_type {
		return static_cast<value_type>(m_value + index);
	}
	constexpr auto operator->() const noexcept {
		return &m_value;
	}
	
	friend constexpr auto operator-(integer_range_iterator const & lhs, integer_range_iterator const & rhs) noexcept -> difference_type {
		static_assert(
			std::is_same<decltype(lhs.m_value - rhs.m_value), difference_type>::value,
			"Incorrect difference_type."
		);
		return lhs.m_value - rhs.m_value;
	}
	friend constexpr auto operator+(integer_range_iterator const & lhs, difference_type const & rhs) -> integer_range_iterator {
		return integer_range_iterator(static_cast<underlying_type>(lhs.m_value + rhs));
	}
	
	friend constexpr auto operator==(integer_range_iterator const & lhs, integer_range_iterator const & rhs) noexcept -> bool {
		return lhs.m_value == rhs.m_value;
	}
	friend constexpr auto operator<(integer_range_iterator const & lhs, integer_range_iterator const & rhs) noexcept -> bool {
		return lhs.m_value < rhs.m_value;
	}
private:
	underlying_type m_value;
};

template<typename T>
constexpr auto operator+(typename integer_range_iterator<T>::difference_type const & lhs, integer_range_iterator<T> const & rhs) {
	return rhs + lhs;
}
template<typename T>
decltype(auto) operator+=(integer_range_iterator<T> & lhs, typename integer_range_iterator<T>::difference_type const & rhs) {
	return lhs = lhs + rhs;
}
template<typename T>
constexpr auto operator-(integer_range_iterator<T> const & lhs, typename integer_range_iterator<T>::difference_type const & rhs) {
	return lhs + (-rhs);
}
template<typename T>
decltype(auto) operator-=(integer_range_iterator<T> & lhs, typename integer_range_iterator<T>::difference_type const & rhs) {
	return lhs = lhs - rhs;
}

}	// namespace detail

template<typename Iterator>
struct range_type {
	using const_iterator = Iterator;
	using const_reverse_iterator = std::reverse_iterator<const_iterator>;

	using value_type = typename const_iterator::value_type;
	using index_type = typename const_iterator::index_type;
	using difference_type = typename const_iterator::difference_type;
	using size_type = integer<0, detail::range_of_type<value_type>()>;

	template<typename First, typename Last>
	constexpr range_type(First && first, Last && last) noexcept:
		m_begin(std::forward<First>(first)),
		m_end(std::forward<Last>(last)) {
	}

	constexpr auto begin() const noexcept {
		return m_begin;
	}
	constexpr auto cbegin() const noexcept {
		return begin();
	}
	constexpr auto end() const noexcept {
		return m_end;
	}
	constexpr auto cend() const noexcept {
		return end();
	}
	constexpr auto rbegin() const noexcept {
		return const_reverse_iterator(end());
	}
	constexpr auto crbegin() const noexcept {
		return rbegin();
	}
	constexpr auto rend() const noexcept {
		return const_reverse_iterator(begin());
	}
	constexpr auto crend() const noexcept {
		return rend();
	}

	constexpr auto front() const -> value_type {
		return *begin();
	}
	constexpr auto back() const -> value_type {
		return *(end() - constant<1>);
	}
	constexpr auto operator[](index_type const & index) const -> value_type {
		return begin()[index];
	}
	template<typename Index>
	constexpr auto at(Index const & index) const -> value_type {
		return begin()[static_cast<index_type>(index)];
	}

	constexpr auto size() const {
		return size_type(end() - begin());
	}
	constexpr auto max_size() const {
		return constant<detail::range_of_type<value_type>()>;
	}
	constexpr auto empty() const {
		return begin() == end();
	}

private:
	const_iterator m_begin;
	const_iterator m_end;
};

template<typename T>
using integer_range_type = range_type<detail::integer_range_iterator<T>>;

// If end is less than begin, the behavior is undefined.
template<typename Begin, typename End>
constexpr auto integer_range(Begin && begin, End && end) noexcept {
	using integer_type = integer<
		static_cast<intmax_t>(std::numeric_limits<std::decay_t<Begin>>::min()),
		static_cast<intmax_t>(std::numeric_limits<std::decay_t<End>>::max())
	>;
	return integer_range_type<integer_type>(std::forward<Begin>(begin), std::forward<End>(end));
}

template<typename Size>
constexpr auto integer_range(Size && size) noexcept {
	return integer_range(constant<0>, std::forward<Size>(size));
}

}	// namespace bounded
