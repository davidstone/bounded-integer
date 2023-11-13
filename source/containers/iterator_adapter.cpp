// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

module;

#include <operators/arrow.hpp>
#include <operators/forward.hpp>

export module containers.iterator_adapter;

import containers.contiguous_iterator;
import containers.default_adapt_traits;
import containers.is_iterator;
import containers.iter_difference_t;
import containers.iterator_category_base;
import containers.reference_wrapper;

import bounded;
import operators;
import std_module;

export import containers.common_iterator_functions;

using namespace bounded::literal;

namespace containers {

template<typename T>
struct adapted_difference_type {
};

template<iterator T>
struct adapted_difference_type<T> {
	// TODO: technically not correct. For instance, consider an iterator adapter
	// that visits each element twice.
	using difference_type = iter_difference_t<T>;
};

export template<typename Iterator, typename Traits>
struct adapt_iterator;

template<typename T>
using unwrapped_t = decltype(::containers::unwrap(bounded::declval<T>()));

template<typename Iterator, typename Traits>
concept adapted_dereferenceable = requires(Iterator const & it, unwrapped_t<Traits> const traits) {
	traits.dereference(it);
};

template<typename Offset, typename Iterator, typename Traits>
concept adapted_addable = requires(Offset const offset, Iterator it, unwrapped_t<Traits> const traits) {
	adapt_iterator<Iterator, Traits>(traits.add(std::move(it), offset), traits);
};

template<typename RHSIterator, typename LHSIterator, typename Traits>
concept adapted_subtractable = requires(LHSIterator lhs, RHSIterator rhs, unwrapped_t<Traits> const traits) {
	traits.subtract(std::move(lhs), std::move(rhs));
};

template<typename RHSIterator, typename LHSIterator, typename Traits>
concept adapted_ordered = requires(LHSIterator const & lhs, RHSIterator const & rhs, unwrapped_t<Traits> const traits) {
	traits.compare(lhs, rhs);
};

template<typename RHSIterator, typename LHSIterator, typename Traits>
concept adapted_equality_comparable = requires(LHSIterator const & lhs, RHSIterator const & rhs, unwrapped_t<Traits> const traits) {
	traits.equal(lhs, rhs);
};

template<typename Traits>
constexpr auto can_store_copy =
	bounded::mostly_trivial<Traits> and
	sizeof(Traits) <= sizeof(std::reference_wrapper<Traits>);

template<typename Traits>
using stored_traits = std::conditional_t<
	can_store_copy<Traits>,
	std::remove_const_t<Traits>,
	std::reference_wrapper<Traits const>
>;


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

export template<typename Iterator, typename Traits>
struct adapt_iterator : iterator_category_base<Iterator>, adapted_difference_type<Iterator> {
	adapt_iterator() = default;

	constexpr adapt_iterator(Iterator it, auto && traits):
		m_base(std::move(it)),
		m_traits(OPERATORS_FORWARD(traits))
	{
	}

	template<bounded::convertible_to<Iterator> It>
	constexpr adapt_iterator(adapt_iterator<It, Traits> other):
		m_base(std::move(other).base()),
		m_traits(std::move(other).traits())
	{
	}
	
	constexpr auto base() const & -> Iterator const & {
		return m_base;
	}
	constexpr auto base() && -> Iterator && {
		return std::move(m_base);
	}

	constexpr auto traits() const -> auto && {
		return ::containers::unwrap(m_traits);
	}

	OPERATORS_ARROW_DEFINITIONS

	friend constexpr auto operator*(adapt_iterator const & it) -> decltype(auto) requires adapted_dereferenceable<Iterator, Traits> {
		return it.traits().dereference(it.base());
	}

	friend constexpr auto operator+(adapt_iterator lhs, adapted_addable<Iterator, Traits> auto const rhs) -> adapt_iterator {
		return adapt_iterator(lhs.traits().add(std::move(lhs).base(), rhs), lhs.traits());
	}


	template<adapted_subtractable<Iterator, Traits> RHSIterator>
	friend constexpr auto operator-(adapt_iterator lhs, adapt_iterator<RHSIterator, Traits> rhs) {
		return lhs.traits().subtract(std::move(lhs).base(), std::move(rhs).base());
	}

	template<adapted_ordered<Iterator, Traits> RHSIterator>
	friend constexpr auto operator<=>(adapt_iterator const & lhs, adapt_iterator<RHSIterator, Traits> const & rhs) {
		return lhs.traits().compare(lhs.base(), rhs.base());
	}

	template<adapted_equality_comparable<Iterator, Traits> RHSIterator>
	friend constexpr auto operator==(adapt_iterator const & lhs, adapt_iterator<RHSIterator, Traits> const & rhs) -> bool {
		return lhs.traits().equal(lhs.base(), rhs.base());
	}

private:
	[[no_unique_address]] Iterator m_base;
	[[no_unique_address]] stored_traits<Traits> m_traits;
};

template<typename Iterator, typename Traits>
adapt_iterator(Iterator, Traits &&) -> adapt_iterator<Iterator, std::remove_cvref_t<Traits>>;

} // namespace containers

struct traits :
	containers::default_dereference,
	containers::default_add,
	containers::default_subtract,
	containers::default_compare
{
};

template<typename T>
using iterator = containers::adapt_iterator<containers::contiguous_iterator<T, 2_bi>, traits>;

using const_iterator = iterator<int const>;
using mutable_iterator = iterator<int>;

static_assert(bounded::convertible_to<const_iterator, const_iterator>);
static_assert(bounded::convertible_to<mutable_iterator, mutable_iterator>);
static_assert(bounded::convertible_to<mutable_iterator, const_iterator>);
static_assert(!bounded::convertible_to<const_iterator, mutable_iterator>);