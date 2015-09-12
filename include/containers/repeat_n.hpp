// Create a range that creates n copies of a value
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

#include <bounded_integer/bounded_integer.hpp>

#include <iterator>
#include <utility>

namespace containers {
namespace detail {

template<typename Size, typename T>
struct repeat_n_t;

struct repeat_n_sentinel {};

template<typename Size, typename T>
struct repeat_n_iterator {
	using iterator_category = std::random_access_iterator_tag;
	using value_type = std::remove_reference_t<T> const;
	using difference_type = decltype(std::declval<Size>() - std::declval<Size>());
	using pointer = value_type *;
	using reference = value_type &;
	
	constexpr auto const & operator*() const {
		return m_value.get();
	}
	constexpr auto operator->() const {
		return std::addressof(operator*());
	}

	friend constexpr auto operator==(repeat_n_iterator const lhs, repeat_n_iterator const rhs) {
		return lhs.m_remaining == rhs.m_remaining;
	}
	friend constexpr auto operator==(repeat_n_iterator const lhs, repeat_n_sentinel) {
		return lhs.m_remaining == bounded::constant<0>;
	}
	friend constexpr auto operator<(repeat_n_iterator const lhs, repeat_n_iterator const rhs) {
		return lhs.m_remaining < rhs.m_remaining;
	}

	template<typename Offset>
	friend constexpr auto operator+(repeat_n_iterator const it, Offset const offset) {
		return repeat_n_iterator(Size(it.m_remaining - offset), it.m_value);
	}
	friend constexpr auto operator-(repeat_n_iterator const lhs, repeat_n_iterator const rhs) {
		return rhs.m_remaining - lhs.m_remaining;
	}
	friend constexpr auto operator-(repeat_n_sentinel, repeat_n_iterator const rhs) {
		return rhs.m_remaining;
	}

private:
	friend struct repeat_n_t<Size, T>;
	constexpr repeat_n_iterator(Size const remaining, value_type & value):
		m_remaining(remaining),
		m_value(value)
	{
	}

	Size m_remaining;
	std::reference_wrapper<value_type> m_value;
};

template<typename Size, typename T>
constexpr auto operator==(repeat_n_sentinel lhs, repeat_n_iterator<Size, T> const rhs) {
	return rhs == lhs;
}
template<typename Size, typename T>
constexpr auto operator!=(repeat_n_iterator<Size, T> const lhs, repeat_n_iterator<Size, T> const rhs) {
	return !(lhs == rhs);
}
template<typename Size, typename T>
constexpr auto operator!=(repeat_n_iterator<Size, T> const lhs, repeat_n_sentinel const rhs) {
	return !(lhs == rhs);
}
template<typename Size, typename T>
constexpr auto operator!=(repeat_n_sentinel const lhs, repeat_n_iterator<Size, T> const rhs) {
	return !(lhs == rhs);
}

template<typename Size, typename T>
constexpr auto operator<(repeat_n_iterator<Size, T> const lhs, repeat_n_sentinel const rhs) {
	// It is not possible to be greater than the sentinel. The iterator is
	// either less than it or equal to it.
	return lhs != rhs;
}


template<typename Size, typename T, typename Offset>
constexpr auto & operator+=(repeat_n_iterator<Size, T> & it, Offset const offset) {
	it = it + offset;
	return it;
}

template<typename Size, typename T>
constexpr auto & operator++(repeat_n_iterator<Size, T> & it) {
	return it += bounded::constant<1>;
}
template<typename Size, typename T>
constexpr auto operator++(repeat_n_iterator<Size, T> & it, int) {
	auto temp = it;
	++it;
	return temp;
}



template<typename Size, typename T>
struct repeat_n_t {
	using size_type = Size;
	using value_type = T;
	using index_type = bounded::checked_integer<0, static_cast<std::intmax_t>(std::numeric_limits<size_type>::max()) - 1>;

	using const_iterator = repeat_n_iterator<size_type, value_type>;
	
	template<typename U>
	repeat_n_t(size_type const size, U && value):
		m_size(size),
		m_value(std::forward<U>(value))
	{
	}

	constexpr auto begin() const noexcept {
		return const_iterator(m_size, m_value);
	}
	constexpr auto end() const noexcept {
		return repeat_n_sentinel{};
	}

	constexpr auto && operator[](index_type const & index) const {
		return *(begin() + index);
	}

private:
	size_type m_size;
	value_type m_value;
};


template<typename Size, typename T>
constexpr auto repeat_n(Size const size, T && value) {
	using size_type = bounded::integer<0, static_cast<std::intmax_t>(std::numeric_limits<Size>::max())>;
	// T is either an lvalue reference or not a reference
	return repeat_n_t<size_type, T>(size, std::forward<T>(value));
}

}	// namespace detail
}	// namespace containers