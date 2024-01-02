// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

module;

#include <operators/arrow.hpp>
#include <operators/forward.hpp>

export module containers.adapt_iterator;

import containers.contiguous_iterator;
import containers.default_adapt_traits;
import containers.iter_difference_t;
import containers.iterator;
import containers.iterator_category_base;
import containers.reference_or_value;

import bounded;
import operators;
import std_module;

export import containers.common_iterator_functions;

using namespace bounded::literal;

namespace containers {

template<typename Sentinel>
struct adapt_sentinel {
	adapt_sentinel() = default;

	constexpr adapt_sentinel(Sentinel sentinel):
		m_base(std::move(sentinel))
	{
	}

	template<bounded::convertible_to<Sentinel> It>
	constexpr adapt_sentinel(adapt_sentinel<It> other):
		m_base(std::move(other).base())
	{
	}
	
	constexpr auto base() const & -> Sentinel const & {
		return m_base;
	}
	constexpr auto base() && -> Sentinel && {
		return std::move(m_base);
	}

private:
	[[no_unique_address]] Sentinel m_base;
};

template<typename T>
struct adapted_difference_type {
};

template<iterator T>
struct adapted_difference_type<T> {
	// TODO: technically not correct. For instance, consider an iterator adapter
	// that visits each element twice.
	using difference_type = iter_difference_t<T>;
};

template<typename T>
using unwrap_t = decltype(::containers::unwrap(bounded::declval<T>()));

template<typename Offset, typename Iterator, typename Traits>
concept adapted_addable = requires(Offset const offset, Iterator it, Traits const traits) {
	{ traits.add(std::move(it), offset) } -> bounded::convertible_to<Iterator>;
};

template<typename LHSIterator, typename RHSIterator, typename Traits>
concept adapted_subtractable = requires(LHSIterator const lhs, RHSIterator const rhs, Traits const traits) {
	traits.subtract(lhs, rhs);
};

template<typename RHSIterator, typename LHSIterator, typename Traits>
concept adapted_ordered = requires(LHSIterator const & lhs, RHSIterator const & rhs, Traits const traits) {
	traits.compare(lhs, rhs);
};

template<typename RHSIterator, typename LHSIterator, typename Traits>
concept adapted_equality_comparable = requires(LHSIterator const & lhs, RHSIterator const & rhs, Traits const traits) {
	traits.equal(lhs, rhs);
};

template<typename Traits, typename Iterator>
concept iterator_traits =
	adapted_addable<bounded::constant_t<1>, Iterator, unwrap_t<Traits>> and
	requires(unwrap_t<Traits> const traits, Iterator it) { traits.dereference(it); };

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

export template<iterator Iterator, iterator_traits<Iterator> TraitsStorage>
struct adapt_iterator :
	iterator_category_base<Iterator>,
	adapted_difference_type<Iterator>
{
private:
	using Traits = unwrap_t<TraitsStorage>;
public:
	adapt_iterator() = default;

	constexpr adapt_iterator(Iterator it, TraitsStorage traits):
		m_base(std::move(it)),
		m_traits(std::move(traits))
	{
	}

	template<bounded::convertible_to<Iterator> It>
	constexpr adapt_iterator(adapt_iterator<It, TraitsStorage> other):
		m_base(std::move(other).base()),
		m_traits(other.traits())
	{
	}
	
	constexpr auto base() const & -> Iterator const & {
		return m_base;
	}
	constexpr auto base() && -> Iterator && {
		return std::move(m_base);
	}

	constexpr auto traits() const & -> auto && {
		return ::containers::unwrap(m_traits);
	}
	constexpr auto traits() && -> auto && {
		return ::containers::unwrap(std::move(m_traits));
	}

	OPERATORS_ARROW_DEFINITIONS

	friend constexpr auto operator*(adapt_iterator const & it) -> decltype(auto) {
		return it.traits().dereference(it.base());
	}

	friend constexpr auto operator+(adapt_iterator lhs, adapted_addable<Iterator, Traits> auto const rhs) -> adapt_iterator {
		auto base = lhs.traits().add(std::move(lhs).base(), rhs);
		return adapt_iterator(std::move(base), std::move(lhs).traits());
	}


	template<adapted_subtractable<Iterator, Traits> LHSIterator>
	friend constexpr auto operator-(
		adapt_iterator<LHSIterator, TraitsStorage> const & lhs,
		adapt_iterator const & rhs
	) {
		return rhs.traits().subtract(lhs.base(), rhs.base());
	}

	template<adapted_subtractable<Iterator, Traits> Sentinel>
	friend constexpr auto operator-(adapt_sentinel<Sentinel> const & lhs, adapt_iterator const & rhs) {
		return rhs.traits().subtract(lhs.base(), rhs.base());
	}

	template<adapted_ordered<Iterator, Traits> RHSIterator>
	friend constexpr auto operator<=>(adapt_iterator const & lhs, adapt_iterator<RHSIterator, TraitsStorage> const & rhs) {
		return lhs.traits().compare(lhs.base(), rhs.base());
	}

	template<adapted_ordered<Iterator, Traits> Sentinel>
	friend constexpr auto operator<=>(adapt_iterator const & lhs, adapt_sentinel<Sentinel> const & rhs) {
		return lhs.traits().compare(lhs.base(), rhs.base());
	}

	template<adapted_equality_comparable<Iterator, Traits> RHSIterator>
	friend constexpr auto operator==(adapt_iterator const & lhs, adapt_iterator<RHSIterator, TraitsStorage> const & rhs) -> bool {
		return lhs.traits().equal(lhs.base(), rhs.base());
	}

	template<adapted_equality_comparable<Iterator, Traits> Sentinel>
	friend constexpr auto operator==(adapt_iterator const & lhs, adapt_sentinel<Sentinel> const & rhs) -> bool {
		return lhs.traits().equal(lhs.base(), rhs.base());
	}

private:
	[[no_unique_address]] Iterator m_base;
	[[no_unique_address]] TraitsStorage m_traits;
};

export template<typename Iterator, typename TraitsStorage>
constexpr auto adapt_iterator_or_sentinel(Iterator it, TraitsStorage traits) {
	if constexpr (iterator<Iterator>) {
		return adapt_iterator(
			std::move(it),
			std::move(traits)
		);
	} else {
		return adapt_sentinel<Iterator>(std::move(it));
	}
}

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