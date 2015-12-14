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

#include <containers/addressof.hpp>
#include <containers/common_iterator_functions.hpp>
#include <containers/moving_vector/forward_declaration.hpp>
#include <containers/vector/vector.hpp>

#include <bounded_integer/bounded_integer.hpp>

#include <iterator>
#include <type_traits>

namespace containers {
namespace detail {

template<typename T, typename Container>
struct moving_vector_iterator {
	using value_type = T;

private:
	using base_iterator = std::conditional_t<std::is_const<value_type>::value, typename Container::const_moving_iterator, typename Container::moving_iterator>;
	using element_type = typename base_iterator::value_type;

public:
	using difference_type = typename base_iterator::difference_type;
	using pointer = value_type *;
	using reference = value_type &;
	using iterator_category = typename base_iterator::iterator_category;

	constexpr moving_vector_iterator() noexcept = default;

	constexpr explicit moving_vector_iterator(base_iterator const other) noexcept:
		m_it(other) {
	}

	// Convert iterator to const_iterator
	constexpr operator moving_vector_iterator<value_type const, Container>() const noexcept {
		return moving_vector_iterator<value_type const, Container>(m_it);
	}

	// Convert iterator to moving_iterator. Will not work for const_iterator
	constexpr explicit operator typename Container::moving_iterator() const noexcept {
		return { m_it };
	}

	constexpr auto & operator*() const {
		return **m_it;
	}
	constexpr auto operator->() const {
		return ::containers::addressof(operator*());
	}

	template<typename Offset, BOUNDED_REQUIRES(std::numeric_limits<Offset>::is_integer)>
	friend constexpr auto operator+(moving_vector_iterator const it, Offset const offset) {
		return moving_vector_iterator(it.m_it + offset);
	}
	friend constexpr auto operator-(moving_vector_iterator const lhs, moving_vector_iterator const rhs) {
		return static_cast<difference_type>(lhs.m_it - rhs.m_it);
	}

	constexpr auto & operator[](index_type<moving_vector_iterator> const index) const {
		return *(*this + index);
	}

	friend constexpr auto operator==(moving_vector_iterator const lhs, moving_vector_iterator const rhs) noexcept {
		return lhs.m_it == rhs.m_it;
	}
	friend constexpr auto operator<(moving_vector_iterator const lhs, moving_vector_iterator const rhs) noexcept {
		return lhs.m_it < rhs.m_it;
	}
	
private:
	base_iterator m_it;
};

}	// namespace detail
}	// namespace containers
