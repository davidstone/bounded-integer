// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

module;

#include <operators/arrow.hpp>

export module containers.contiguous_iterator;

import containers.is_iterator;
import containers.maximum_array_size;
import containers.offset_type;
export import containers.common_iterator_functions;

import bounded;
import operators;
import std_module;

using namespace bounded::literal;

namespace containers {

template<typename T>
using opposite_const = std::conditional_t<std::is_const_v<T>, std::remove_const_t<T>, T const>;

export template<typename T, array_size_type<T> max_difference>
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
	friend auto operator+(contiguous_iterator, bounded::constant_t<1>) -> contiguous_iterator requires(max_difference == 0) {
		std::unreachable();
	}
	friend auto operator+(contiguous_iterator, bounded::constant_t<-1>) -> contiguous_iterator requires(max_difference == 0) {
		std::unreachable();
	}

	friend constexpr auto operator-(contiguous_iterator const lhs, contiguous_iterator const rhs) -> difference_type {
		return ::bounded::assume_in_range<difference_type>(lhs.to_address() - rhs.to_address());
	}
	friend constexpr auto operator-(contiguous_iterator const lhs, contiguous_iterator<opposite_const<T>, max_difference> const rhs) -> difference_type {
		return ::bounded::assume_in_range<difference_type>(lhs.to_address() - rhs.to_address());
	}

private:
	T * m_ptr = nullptr;
};

} // namespace containers

static_assert(containers::random_access_iterator<containers::contiguous_iterator<int, 5_bi>>);
static_assert(std::same_as<containers::offset_type<containers::contiguous_iterator<int, 5_bi>>, bounded::integer<0, 5>>);

using const_iterator = containers::contiguous_iterator<int const, 0_bi>;
using iterator = containers::contiguous_iterator<int, 0_bi>;

static_assert(const_iterator() == const_iterator());
static_assert(const_iterator() == iterator());
static_assert(iterator() == const_iterator());
static_assert(iterator() == iterator());

static_assert(const_iterator() + 0_bi == const_iterator());
static_assert(iterator() + 0_bi == iterator());

static_assert(const_iterator() - const_iterator() == 0_bi);
static_assert(const_iterator() - iterator() == 0_bi);
static_assert(iterator() - const_iterator() == 0_bi);
static_assert(iterator() - iterator() == 0_bi);
