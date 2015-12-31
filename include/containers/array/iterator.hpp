// Copyright David Stone 2015.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/common_iterator_functions.hpp>

#include <bounded/integer.hpp>

#include <cstddef>
#include <cstdint>
#include <iterator>
#include <limits>

namespace containers {
namespace detail {

// It looks like we do not need to pass in type T here, as it is
// Container::value_type, but for const_iterator, it is actually
// Container::value_type const. An alternative implementation would have two
// template parameters: typename Container, bool is_const
template<typename T, typename Container>
struct basic_array_iterator {
	using value_type = T;
	using difference_type = basic_difference_type<typename Container::size_type>;
	using pointer = value_type *;
	using reference = value_type &;
	using iterator_category = std::random_access_iterator_tag;

	constexpr basic_array_iterator() noexcept = default;
	// Convert iterator to const_iterator
	constexpr operator basic_array_iterator<value_type const, Container>() const noexcept {
		return basic_array_iterator<value_type const, Container>(m_it);
	}

	constexpr auto & operator*() const noexcept {
		return *m_it;
	}
	constexpr auto operator->() const noexcept {
		return ::bounded::addressof(operator*());
	}

	friend constexpr auto operator+(basic_array_iterator const lhs, difference_type const rhs) {
		return basic_array_iterator(lhs.m_it + rhs);
	}
	friend constexpr auto operator-(basic_array_iterator const lhs, basic_array_iterator const rhs) {
		return static_cast<difference_type>(lhs.m_it - rhs.m_it);
	}

	constexpr auto & operator[](index_type<basic_array_iterator> const index) const {
		return *(*this + index);
	}

	friend constexpr auto operator==(basic_array_iterator const lhs, basic_array_iterator const rhs) noexcept {
		return lhs.m_it == rhs.m_it;
	}
	friend constexpr auto operator<(basic_array_iterator const lhs, basic_array_iterator const rhs) noexcept {
		return lhs.m_it < rhs.m_it;
	}

private:
	friend Container;
	friend basic_array_iterator<std::remove_const_t<T>, Container>;

	constexpr explicit basic_array_iterator(pointer const other) noexcept:
		m_it(other) {
	}

	pointer m_it = nullptr;
};


}	// namespace detail
}	// namespace containers
