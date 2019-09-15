// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/common_functions.hpp>
#include <containers/range_view.hpp>

#include <bounded/assert.hpp>
#include <bounded/integer.hpp>

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
	using value_type = std::remove_reference_t<T>;
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
	constexpr auto operator->() const {
		return std::addressof(operator*());
	}
	constexpr auto is_end() const {
		return m_is_end;
	}

	friend constexpr auto operator+(single_element_iterator lhs, bounded::constant_t<1>) {
		BOUNDED_ASSERT_OR_ASSUME(!lhs.m_is_end);
		lhs.m_is_end = true;
		return lhs;
	}

	friend constexpr auto operator-(single_element_iterator lhs, bounded::constant_t<1>) {
		BOUNDED_ASSERT_OR_ASSUME(lhs.m_is_end);
		lhs.m_is_end = false;
		return lhs;
	}

private:
	std::remove_reference_t<T> * m_value;
	bool m_is_end = false;
};
template<typename T>
single_element_iterator(T &&) -> single_element_iterator<T &&>;

template<typename T>
constexpr auto compare(single_element_iterator<T> const lhs, single_element_iterator<T> const rhs) {
	return compare(lhs.is_end(), rhs.is_end());
}

template<typename T>
constexpr auto compare(single_element_iterator<T> const lhs, single_element_sentinel) {
	return compare(lhs.is_end(), true);
}
template<typename T>
constexpr auto compare(single_element_sentinel, single_element_iterator<T> const rhs) {
	return compare(true, rhs.is_end());
}

template<typename T>
constexpr auto operator==(single_element_iterator<T> const lhs, single_element_iterator<T> const rhs) {
	return lhs.is_end() == rhs.is_end();
}

template<typename T>
constexpr auto operator==(single_element_iterator<T> const lhs, single_element_sentinel) {
	return lhs.is_end();
}
template<typename T>
constexpr auto operator==(single_element_sentinel, single_element_iterator<T> const rhs) {
	return rhs.is_end();
}

template<typename T>
constexpr auto operator-(single_element_iterator<T> const lhs, single_element_iterator<T> const rhs) {
	BOUNDED_ASSERT(lhs >= rhs);
	return BOUNDED_CONDITIONAL(lhs == rhs, 0_bi, 1_bi);
}

template<typename T>
constexpr auto operator-(single_element_sentinel, single_element_iterator<T> const rhs) {
	return BOUNDED_CONDITIONAL(rhs.is_end(), 0_bi, 1_bi);
}

template<typename T>
constexpr auto operator-(single_element_iterator<T> const lhs, single_element_sentinel) {
	BOUNDED_ASSERT(lhs.is_end());
	return 0_bi;
}

}	// namespace detail

// Note: the element must outlive the range. The range stores a reference
template<typename T>
constexpr auto single_element_range(T && value) {
	return range_view(detail::single_element_iterator(BOUNDED_FORWARD(value)), detail::single_element_sentinel{});
}

}	// namespace containers
