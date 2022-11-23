// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/common_iterator_functions.hpp>
#include <containers/is_iterator.hpp>
#include <containers/iterator_category_base.hpp>
#include <containers/reference_wrapper.hpp>

#include <operators/operators.hpp>

#include <type_traits>
#include <utility>

namespace containers {
namespace detail {

template<typename T>
struct adapted_difference_type {
};

template<iterator T>
struct adapted_difference_type<T> {
	// TODO: technically not correct. For instance, consider an iterator adapter
	// that visits each element twice.
	using difference_type = iter_difference_t<T>;
};

} // namespace detail

// There are a few functions of interest for an iterator:
// 1) Dereferencing: *it
// 2) Add: it + 5_bi
// 3) Subtract: it1 - it2
// 4) Equal: it1 == it2
// 5) Compare: it1 <=> it2
// This allows you to customize those. Your function is passed an iterator.
// Every other function can be built up from these behaviors. Adding,
// subtracting, comparing, and equality are all linked to the same algorithm,
// but unfortunately, there does not appear to be a way to provide a function
// that unifies these operations.

template<typename Iterator, typename Traits>
struct adapt_iterator : detail::iterator_category_base<Iterator>, detail::adapted_difference_type<Iterator> {
	adapt_iterator() = default;

	constexpr adapt_iterator(Iterator it, Traits traits):
		m_base(std::move(it)),
		m_traits(std::move(traits))
	{
	}

	template<bounded::convertible_to<Iterator> It>
	constexpr adapt_iterator(adapt_iterator<It, Traits> other):
		m_base(std::move(other).base()),
		m_traits(std::move(other).traits())
	{
	}
	
	constexpr auto base() const {
		return m_base;
	}

	constexpr auto && traits() const {
		return ::containers::detail::unwrap(m_traits);
	}

	OPERATORS_ARROW_DEFINITIONS

private:
	[[no_unique_address]] Iterator m_base;
	[[no_unique_address]] Traits m_traits;
};

template<typename Iterator, typename Traits>
constexpr auto operator*(adapt_iterator<Iterator, Traits> const it) OPERATORS_RETURNS(
	it.traits().dereference(it.base())
)

template<typename Iterator, typename Traits>
constexpr auto operator+(adapt_iterator<Iterator, Traits> const lhs, auto const rhs) OPERATORS_RETURNS(
	adapt_iterator<Iterator, Traits>(lhs.traits().add(lhs.base(), rhs), lhs.traits())
)


template<typename LHSIterator, typename RHSIterator, typename Traits>
constexpr auto operator-(adapt_iterator<LHSIterator, Traits> const lhs, adapt_iterator<RHSIterator, Traits> const rhs) OPERATORS_RETURNS(
	lhs.traits().subtract(lhs.base(), rhs.base())
)

template<typename LHSIterator, typename RHSIterator, typename Traits>
constexpr auto operator<=>(adapt_iterator<LHSIterator, Traits> const lhs, adapt_iterator<RHSIterator, Traits> const rhs) OPERATORS_RETURNS(
	lhs.traits().compare(lhs.base(), rhs.base())
)

template<typename LHSIterator, typename RHSIterator, typename Traits>
constexpr auto operator==(adapt_iterator<LHSIterator, Traits> const lhs, adapt_iterator<RHSIterator, Traits> const rhs) OPERATORS_RETURNS(
	lhs.traits().equal(lhs.base(), rhs.base())
)

}	// namespace containers
