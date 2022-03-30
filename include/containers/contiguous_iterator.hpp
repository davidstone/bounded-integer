// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/common_functions.hpp>

#include <bounded/integer.hpp>

#include <bounded/assert.hpp>
#include <bounded/unreachable.hpp>

#include <operators/operators.hpp>

#include <cstddef>

namespace containers {

template<typename T, std::ptrdiff_t max_difference>
struct contiguous_iterator {
	using difference_type = bounded::integer<bounded::normalize<-max_difference>, bounded::normalize<max_difference>>;

	contiguous_iterator() = default;
	constexpr explicit contiguous_iterator(T * ptr):
		m_ptr(ptr)
	{
	}

	// Convert iterator to const_iterator
	constexpr operator contiguous_iterator<T const, max_difference>() const {
		return contiguous_iterator<T const, max_difference>(m_ptr);
	}

	constexpr auto & operator*() const {
		return *m_ptr;
	}
	OPERATORS_ARROW_DEFINITIONS

	constexpr auto to_address() const {
		return m_ptr;
	}
	friend auto operator<=>(contiguous_iterator, contiguous_iterator) = default;
private:
	T * m_ptr = nullptr;
};


template<typename T, std::ptrdiff_t max_difference>
constexpr auto operator+(
	contiguous_iterator<T, max_difference> const lhs,
	typename contiguous_iterator<T, max_difference>::difference_type const rhs
) {
	return contiguous_iterator<T, max_difference>(lhs.to_address() + rhs);
}

// Iterators over ranges of zero size tend to come up only in generic code, and
// it is annoying to be unable to do things like use a range-based for loop over
// an empty array
// Specifying the return type triggers a gcc bug where it thinks that parameter
// pack lengths are mismatched in the constructor of tuple.
template<typename T>
constexpr auto operator+(
	contiguous_iterator<T, 0> const lhs,
	bounded::constant_t<1>
) -> decltype(lhs) {
	bounded::unreachable();
}
template<typename T>
constexpr auto operator+(
	contiguous_iterator<T, 0> const lhs,
	bounded::constant_t<-1>
) -> decltype(lhs) {
	bounded::unreachable();
}


template<typename LHS, typename RHS, std::ptrdiff_t max_difference> requires std::same_as<LHS const, RHS const>
constexpr auto operator-(contiguous_iterator<LHS, max_difference> const lhs, contiguous_iterator<RHS, max_difference> const rhs) {
	using difference_t = typename contiguous_iterator<LHS, max_difference>::difference_type;
	return ::bounded::assume_in_range<difference_t>(lhs.to_address() - rhs.to_address());
}

} // namespace containers
