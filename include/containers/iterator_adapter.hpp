// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/common_iterator_functions.hpp>
#include <containers/index_type.hpp>
#include <containers/reference_wrapper.hpp>

#include <bounded/integer.hpp>

#include <operators/operators.hpp>

#include <iterator>
#include <type_traits>
#include <utility>

namespace containers {

struct default_begin_end {
	static constexpr auto get_begin(auto && range) {
		return begin(OPERATORS_FORWARD(range));
	}

	static constexpr auto get_end(auto && range) {
		return end(OPERATORS_FORWARD(range));
	}
};

struct default_dereference {
	static constexpr decltype(auto) dereference(iterator auto const & it) {
		return *it;
	}
};

struct default_add {
	static constexpr auto add(iterator auto it, auto const & offset) OPERATORS_RETURNS(
		std::move(it) + offset
	)
};

struct default_subtract {
	static constexpr auto subtract(iterator auto const & lhs, iterator auto const & rhs) OPERATORS_RETURNS(
		lhs - rhs
	)
};

struct default_compare {
	template<typename LHS, typename RHS> requires bounded::ordered<LHS, RHS>
	static constexpr auto compare(LHS const & lhs, RHS const & rhs) {
		return lhs <=> rhs;
	}
	template<typename LHS, typename RHS> requires bounded::equality_comparable<LHS, RHS>
	static constexpr auto equal(LHS const & lhs, RHS const & rhs) {
		return lhs == rhs;
	}
};

namespace detail {

template<typename Iterator, typename Traits>
concept adapted_random_access_iterator = requires(Iterator it, Traits traits) { traits.subtract(it, it); };

template<typename Iterator, typename Traits>
concept adapted_bidirectional_iterator = requires(Iterator it, Traits traits) {
	traits.add(it, bounded::constant<1>);
	traits.add(it, bounded::constant<-1>);
};

template<typename Iterator, typename Traits>
concept adapted_forward_iterator = requires(Iterator it, Traits traits) { traits.add(it, bounded::constant<1>); };

// This provides the nested typedefs iterator_category and difference_type if
// and only if Iterator is an iterator, not a sentinel
template<typename Iterator, typename Traits>
struct iterator_typedefs_base {
};

template<typename Iterator, typename Traits> requires iterator<Iterator>
struct iterator_typedefs_base<Iterator, Traits> {
private:
	using base_category = typename std::iterator_traits<Iterator>::iterator_category;
public:
	using value_type = decltype(containers::unwrap(std::declval<Traits>()).dereference(std::declval<Iterator>()));

	// Not sure what these actually mean...
	using pointer = std::remove_reference_t<value_type> *;
	using reference = value_type;

	// TODO: technically not correct. For instance, consider an iterator adapter
	// that visits each element twice.
	using difference_type = typename std::iterator_traits<Iterator>::difference_type;
	using iterator_category =
		std::conditional_t<std::is_same_v<base_category, std::output_iterator_tag>, std::output_iterator_tag,
		std::conditional_t<std::is_same_v<base_category, std::input_iterator_tag>, std::input_iterator_tag,
		std::conditional_t<detail::adapted_random_access_iterator<Iterator, Traits>, std::random_access_iterator_tag,
		std::conditional_t<detail::adapted_bidirectional_iterator<Iterator, Traits>, std::bidirectional_iterator_tag,
		std::conditional_t<detail::adapted_forward_iterator<Iterator, Traits>, std::forward_iterator_tag,
		void
	>>>>>;
	static_assert(not std::is_void_v<iterator_category>);
};

}	// namespace detail

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
struct adapt_iterator :
	detail::iterator_typedefs_base<Iterator, decltype(containers::unwrap(std::declval<Traits>()))>
{
	static_assert(std::is_copy_assignable_v<Iterator>);
	static_assert(std::is_copy_assignable_v<Traits>);

	adapt_iterator() = default;
	constexpr adapt_iterator(Iterator it, Traits traits):
		m_base(std::move(it)),
		m_traits(std::move(traits))
	{
	}
	
	constexpr auto base() const {
		return m_base;
	}

	constexpr auto && traits() const {
		return containers::unwrap(m_traits);
	}

	OPERATORS_ARROW_DEFINITIONS
	OPERATORS_BRACKET_ITERATOR_DEFINITIONS

private:
	[[no_unique_address]] Iterator m_base;
	[[no_unique_address]] Traits m_traits;
};


template<typename Iterator, typename Traits>
constexpr decltype(auto) operator*(adapt_iterator<Iterator, Traits> const it) {
	return it.traits().dereference(it.base());
}

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
