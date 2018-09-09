// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

// Adapter for functions that cannot be made compatible with a bounded::integer

#pragma once

#include <containers/common_iterator_functions.hpp>
#include <containers/is_iterator.hpp>
#include <containers/operator_arrow.hpp>

#include <bounded/integer.hpp>

#include <iterator>

namespace containers {

template<typename Iterator>
struct legacy_iterator : detail::operator_arrow<legacy_iterator<Iterator>> {
	using value_type = typename std::iterator_traits<Iterator>::value_type;
	using difference_type = std::ptrdiff_t;
	using pointer = typename std::iterator_traits<Iterator>::pointer;
	using reference = typename std::iterator_traits<Iterator>::reference;
	using iterator_category = typename std::iterator_traits<Iterator>::iterator_category;

	legacy_iterator() = default;
	constexpr legacy_iterator(Iterator it) noexcept(std::is_nothrow_move_constructible_v<Iterator>):
		m_it(std::move(it)) {
	}
	
	constexpr auto base() const noexcept(std::is_nothrow_copy_constructible_v<Iterator>) {
		return m_it;
	}

	constexpr decltype(auto) operator*() const BOUNDED_NOEXCEPT_GCC_BUG(
		*base()
	)
	template<typename Index>
	constexpr decltype(auto) operator[](Index const index) const BOUNDED_NOEXCEPT_GCC_BUG(
		base()[index]
	)

private:
	Iterator m_it;
};

template<typename Iterator, typename Offset, BOUNDED_REQUIRES(std::numeric_limits<Offset>::is_integer)>
constexpr auto operator+(legacy_iterator<Iterator> const lhs, Offset const rhs) BOUNDED_NOEXCEPT(
	legacy_iterator<Iterator>(lhs.base() + static_cast<typename std::iterator_traits<Iterator>::difference_type>(rhs))
)
template<typename Iterator>
constexpr auto operator-(legacy_iterator<Iterator> const lhs, legacy_iterator<Iterator> const rhs) BOUNDED_NOEXCEPT(
	static_cast<typename legacy_iterator<Iterator>::difference_type>(lhs.base() - rhs.base())
)

template<typename Iterator>
constexpr auto compare(legacy_iterator<Iterator> const lhs, legacy_iterator<Iterator> const rhs) BOUNDED_NOEXCEPT(
	compare(lhs.base(), rhs.base())
)

}	// namespace containers
