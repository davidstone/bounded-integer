// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/common_functions.hpp>
#include <containers/offset_type.hpp>
#include <containers/range_view.hpp>

#include <bounded/assert.hpp>
#include <bounded/integer.hpp>

#include <operators/arrow.hpp>

#include <iterator>
#include <type_traits>
#include <utility>

namespace containers {
namespace detail {
using namespace bounded::literal;

struct single_element_sentinel {
};

template<typename T>
struct single_element_iterator {
	static_assert(std::is_reference_v<T>);
	using value_type = std::remove_cvref_t<T>;
	using difference_type = bounded::integer<0, 1>;
	using pointer = value_type *;
	using reference = T;
	using iterator_category = std::output_iterator_tag;

	single_element_iterator() = default;
	constexpr explicit single_element_iterator(T && value):
		m_value(std::addressof(value))
	{
	}
	
	constexpr auto && operator*() const {
		BOUNDED_ASSERT_OR_ASSUME(!m_is_end);
		return static_cast<T>(*m_value);
	}
	OPERATORS_ARROW_DEFINITIONS
	constexpr auto is_end() const {
		return m_is_end;
	}

	friend constexpr auto operator+(single_element_iterator lhs, offset_type<single_element_iterator> const offset) {
		if (offset == 1_bi) {
			BOUNDED_ASSERT_OR_ASSUME(!lhs.m_is_end);
			lhs.m_is_end = true;
		}
		return lhs;
	}

	friend constexpr auto operator-(single_element_iterator lhs, offset_type<single_element_iterator> const offset) {
		if (offset == 1_bi) {
			BOUNDED_ASSERT_OR_ASSUME(lhs.m_is_end);
			lhs.m_is_end = false;
		}
		return lhs;
	}

private:
	std::remove_reference_t<T> * m_value;
	bool m_is_end = false;
};
template<typename T>
single_element_iterator(T &&) -> single_element_iterator<T &&>;

template<typename T>
constexpr auto operator<=>(single_element_iterator<T> const lhs, single_element_iterator<T> const rhs) {
	return lhs.is_end() <=> rhs.is_end();
}

template<typename T>
constexpr auto operator<=>(single_element_iterator<T> const lhs, single_element_sentinel) {
	return lhs.is_end() <=> true;
}

template<typename T>
constexpr auto operator==(single_element_iterator<T> const lhs, single_element_iterator<T> const rhs) -> bool {
	return lhs.is_end() == rhs.is_end();
}

template<typename T>
constexpr auto operator==(single_element_iterator<T> const lhs, single_element_sentinel) -> bool {
	return lhs.is_end();
}

template<typename T>
constexpr auto operator-(single_element_iterator<T> const lhs, single_element_iterator<T> const rhs) {
	return bounded::integer(lhs.is_end()) - bounded::integer(rhs.is_end());
}

template<typename T>
constexpr auto operator-(single_element_sentinel, single_element_iterator<T> const rhs) {
	return 1_bi - bounded::integer(rhs.is_end());
}

template<typename T>
constexpr auto operator-(single_element_iterator<T> const lhs, single_element_sentinel) {
	return bounded::integer(lhs.is_end()) - 1_bi;
}

}	// namespace detail

// Note: the element must outlive the range. The range stores a reference
template<typename T>
constexpr auto single_element_range(T && value) {
	return range_view(detail::single_element_iterator(OPERATORS_FORWARD(value)), detail::single_element_sentinel{});
}

}	// namespace containers
