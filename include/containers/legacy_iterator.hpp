// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

// Adapter for functions that cannot be made compatible with a bounded::integer

#pragma once

#include <containers/common_iterator_functions.hpp>
#include <containers/is_iterator.hpp>
#include <containers/iter_value_t.hpp>
#include <containers/to_address.hpp>

#include <bounded/integer.hpp>

#include <operators/operators.hpp>

#include <iterator>

namespace containers {

template<typename Iterator>
struct legacy_iterator {
	using value_type = iter_value_t<Iterator>;
	using difference_type = std::ptrdiff_t;
	using pointer = typename std::iterator_traits<Iterator>::pointer;
	using reference = typename std::iterator_traits<Iterator>::reference;
	using iterator_category = typename std::iterator_traits<Iterator>::iterator_category;

	legacy_iterator() = default;
	constexpr legacy_iterator(Iterator it):
		m_it(std::move(it)) {
	}
	template<bounded::convertible_to<Iterator> It>
	constexpr legacy_iterator(legacy_iterator<It> other):
		m_it(other.base())
	{
	}
	
	constexpr auto base() const {
		return m_it;
	}

	constexpr decltype(auto) operator*() const {
		return *base();
	}
	OPERATORS_ARROW_DEFINITIONS
	OPERATORS_BRACKET_ITERATOR_DEFINITIONS

	constexpr auto to_address() const requires detail::to_addressable<Iterator> {
		return containers::to_address(m_it);
	}

	friend auto operator<=>(legacy_iterator, legacy_iterator) = default;

private:
	Iterator m_it;
};

template<typename Iterator>
constexpr auto operator+(legacy_iterator<Iterator> const lhs, bounded::integral auto const rhs) {
	return legacy_iterator<Iterator>(lhs.base() + static_cast<typename std::iterator_traits<Iterator>::difference_type>(rhs));
}
template<typename Iterator>
constexpr auto operator-(legacy_iterator<Iterator> const lhs, legacy_iterator<Iterator> const rhs) {
	return static_cast<typename legacy_iterator<Iterator>::difference_type>(lhs.base() - rhs.base());
}

constexpr auto make_legacy_iterator = [](auto it) {
	if constexpr (bounded::bounded_integer<decltype(it - it)>) {
		return containers::legacy_iterator(it);
	} else {
		return it;
	}
};

}	// namespace containers
