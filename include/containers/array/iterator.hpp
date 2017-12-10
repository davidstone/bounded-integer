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

#include <containers/common_iterator_functions.hpp>

#include <bounded_integer/bounded_integer.hpp>

#include <cstddef>
#include <cstdint>
#include <iterator>

namespace containers {
namespace detail {

template<typename T, std::size_t size>
struct array;

template<typename T, std::intmax_t size>
struct basic_array_iterator {
	using value_type = T;
	using difference_type = bounded::integer<-size, size>;
	using pointer = value_type *;
	using reference = value_type &;
	using iterator_category = std::random_access_iterator_tag;

	constexpr basic_array_iterator() noexcept = default;
	// Convert iterator to const_iterator
	constexpr operator basic_array_iterator<value_type const, size>() const noexcept {
		return { m_it };
	}

	constexpr auto & operator*() const {
		return *m_it;
	}
	constexpr auto operator->() const {
		return std::addressof(operator*());
	}
	constexpr auto & operator[](index_type<basic_array_iterator> const index) const {
		return m_it[static_cast<std::size_t>(index)];
	}

	friend constexpr auto operator+(basic_array_iterator const lhs, difference_type const rhs) {
		return basic_array_iterator(lhs.m_it + rhs);
	}
	friend constexpr auto operator-(basic_array_iterator const lhs, basic_array_iterator const rhs) {
		return static_cast<difference_type>(lhs.m_it - rhs.m_it);
	}

	friend constexpr auto operator==(basic_array_iterator const lhs, basic_array_iterator const rhs) noexcept {
		return lhs.m_it == rhs.m_it;
	}
	friend constexpr auto operator<(basic_array_iterator const lhs, basic_array_iterator const rhs) noexcept {
		return lhs.m_it < rhs.m_it;
	}

private:
	friend struct ::containers::detail::array<value_type, size>;
	friend struct ::containers::detail::array<std::remove_const_t<value_type>, size>;
	friend struct basic_array_iterator<std::remove_const_t<value_type>, size>;

	using base_iterator = value_type *;
	constexpr explicit basic_array_iterator(base_iterator const other) noexcept:
		m_it(other) {
	}

	base_iterator m_it;
};


}	// namespace detail
}	// namespace bounded
