// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

module;

#include <operators/arrow.hpp>

export module containers.contiguous_iterator;

import containers.is_iterator;
import containers.offset_type;
export import containers.common_iterator_functions;

import bounded;
import operators;
import std_module;

namespace containers {

template<typename T, typename U>
concept same_as_ignore_const = std::same_as<T const, U const>;

export template<typename T, std::ptrdiff_t max_difference>
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

	friend constexpr auto operator+(contiguous_iterator const lhs, difference_type const rhs) -> contiguous_iterator {
		return contiguous_iterator<T, max_difference>(lhs.to_address() + rhs);
	}

	// Iterators over ranges of zero size tend to come up only in generic code, and
	// it is annoying to be unable to do things like use a range-based for loop over
	// an empty array
	// Specifying the return type triggers a gcc bug where it thinks that parameter
	// pack lengths are mismatched in the constructor of tuple.
	friend auto operator+(contiguous_iterator, bounded::constant_t<1>) -> contiguous_iterator requires(max_difference == 0) {
		std::unreachable();
	}
	friend auto operator+(contiguous_iterator, bounded::constant_t<-1>) -> contiguous_iterator requires(max_difference == 0) {
		std::unreachable();
	}

	template<same_as_ignore_const<T> RHS>
	friend constexpr auto operator-(contiguous_iterator const lhs, contiguous_iterator<RHS, max_difference> const rhs) {
		return ::bounded::assume_in_range<difference_type>(lhs.to_address() - rhs.to_address());
	}

private:
	T * m_ptr = nullptr;
};

} // namespace containers

static_assert(containers::random_access_iterator<containers::contiguous_iterator<int, 5>>);
static_assert(std::same_as<containers::offset_type<containers::contiguous_iterator<int, 5>>, bounded::integer<0, 5>>);
