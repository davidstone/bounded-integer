// array iterator
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

#include "../class.hpp"
#include "../arithmetic/pointer.hpp"
#include "../policy/throw_policy.hpp"

#include <cstddef>
#include <cstdint>
#include <iterator>


namespace bounded {

namespace detail {

template<typename T, std::size_t size>
struct array;

template<typename T, intmax_t size>
struct basic_iterator {
	using value_type = T;
	using difference_type = integer<-size, size>;
	using index_type = integer<0, size - 1, throw_policy<>>;
	using pointer = value_type *;
	using reference = value_type &;
	using iterator_category = std::random_access_iterator_tag;

	constexpr basic_iterator() noexcept = default;
	// Convert iterator to const_iterator
	constexpr operator basic_iterator<T const, size>() const noexcept {
		return basic_iterator<T const, size>(m_it);
	}

	constexpr auto operator*() const -> reference {
		return *m_it;
	}
	constexpr auto operator->() const -> pointer {
		return & operator*();
	}
	constexpr auto operator[](index_type const index) const -> reference {
		return m_it[static_cast<std::size_t>(index)];
	}

	friend constexpr auto operator+(basic_iterator const lhs, difference_type const rhs) -> basic_iterator {
		return basic_iterator(lhs.m_it + rhs);
	}
	friend constexpr auto operator-(basic_iterator const lhs, basic_iterator const rhs) -> difference_type {
		return static_cast<difference_type>(lhs.m_it - rhs.m_it);
	}

	friend constexpr auto operator==(basic_iterator const lhs, basic_iterator const rhs) noexcept -> bool {
		return lhs.m_it == rhs.m_it;
	}
	friend constexpr auto operator<(basic_iterator const lhs, basic_iterator const rhs) noexcept -> bool {
		return lhs.m_it < rhs.m_it;
	}

private:
	template<typename U, std::size_t size_>
	friend struct ::bounded::detail::array;
	template<typename U, intmax_t size_>
	friend struct basic_iterator;

	using base_iterator = value_type *;
	constexpr explicit basic_iterator(base_iterator const other) noexcept:
		m_it(other) {
	}

	base_iterator m_it;
};


template<typename T, intmax_t size>
constexpr auto operator!=(basic_iterator<T, size> const lhs, basic_iterator<T, size> const rhs) noexcept -> bool {
	return !(lhs == rhs);
}

template<typename T, intmax_t size>
constexpr auto operator>(basic_iterator<T, size> const lhs, basic_iterator<T, size> const rhs) noexcept -> bool {
	return rhs < lhs;
}
template<typename T, intmax_t size>
constexpr auto operator<=(basic_iterator<T, size> const lhs, basic_iterator<T, size> const rhs) noexcept -> bool {
	return !(lhs > rhs);
}
template<typename T, intmax_t size>
constexpr auto operator>=(basic_iterator<T, size> const lhs, basic_iterator<T, size> const rhs) noexcept -> bool {
	return !(lhs < rhs);
}


template<typename T, intmax_t size>
constexpr auto operator+(typename basic_iterator<T, size>::difference_type const lhs, basic_iterator<T, size> const rhs) -> basic_iterator<T, size> {
	return rhs + lhs;
}
template<typename T, intmax_t size>
constexpr auto operator-(basic_iterator<T, size> const lhs, typename basic_iterator<T, size>::difference_type const rhs) -> basic_iterator<T, size> {
	return lhs + -rhs;
}


template<typename T, intmax_t size>
auto operator+=(basic_iterator<T, size> & it, typename basic_iterator<T, size>::difference_type const offset) -> basic_iterator<T, size> & {
	return it = it + offset;
}
template<typename T, intmax_t size>
auto operator-=(basic_iterator<T, size> & it, typename basic_iterator<T, size>::difference_type const offset) -> basic_iterator<T, size> & {
	return it += -offset;
}


template<typename T, intmax_t size>
auto operator++(basic_iterator<T, size> & it) -> basic_iterator<T, size> & {
	return it += constant<1>;
}
template<typename T, intmax_t size>
auto operator++(basic_iterator<T, size> & it, int) -> basic_iterator<T, size> {
	auto const original = it;
	++it;
	return original;
}
template<typename T, intmax_t size>
auto operator--(basic_iterator<T, size> & it) -> basic_iterator<T, size> & {
	return it -= constant<1>;
}
template<typename T, intmax_t size>
auto operator--(basic_iterator<T, size> & it, int) -> basic_iterator<T, size> {
	auto const original = it;
	--it;
	return original;
}

}	// namespace detail
}	// namespace bounded
