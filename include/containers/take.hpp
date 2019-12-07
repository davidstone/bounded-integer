// Copyright David Stone 2019.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/common_iterator_functions.hpp>
#include <containers/range_view.hpp>

#include <bounded/detail/forward.hpp>
#include <bounded/integer.hpp>
#include <bounded/unreachable.hpp>

#include <operators/operators.hpp>

#include <iterator>
#include <utility>

namespace containers {

using namespace bounded::literal;

// TODO: Make this an iterator if it can be?
template<typename Sentinel>
struct counted_sentinel {
	constexpr explicit counted_sentinel(Sentinel sentinel):
		m_sentinel(std::move(sentinel))
	{
	}

	constexpr auto base() const {
		return m_sentinel;
	}

private:
	Sentinel m_sentinel;
};

template<typename Iterator, typename Count>
struct counted_iterator {
	using iterator_category = typename std::iterator_traits<Iterator>::iterator_category;
	using value_type = typename std::iterator_traits<Iterator>::value_type;
	using difference_type = decltype(std::declval<Count>() - std::declval<Count>());
	using pointer = typename std::iterator_traits<Iterator>::pointer;
	using reference = typename std::iterator_traits<Iterator>::reference;

	constexpr explicit counted_iterator(Iterator it, Count count):
		m_it(std::move(it)),
		m_count(count)
	{
	}

	constexpr decltype(auto) operator*() const {
		return *m_it;
	}
	OPERATORS_ARROW_DEFINITIONS
	OPERATORS_BRACKET_ITERATOR_DEFINITIONS

	friend constexpr auto operator==(counted_iterator const & lhs, counted_iterator const & rhs) {
		return rhs.m_count == lhs.m_count;
	}
	template<typename Sentinel>
	friend constexpr auto operator==(counted_iterator const & lhs, counted_sentinel<Sentinel> const & rhs) {
		return lhs.m_count == 0_bi or lhs.m_it == rhs.base();
	}
	template<typename Sentinel>
	friend constexpr auto operator==(counted_sentinel<Sentinel> const & lhs, counted_iterator const & rhs) {
		return rhs == lhs;
	}

	// A smaller count means you are further along
	friend constexpr auto operator<=>(counted_iterator const & lhs, counted_iterator const & rhs) {
		return rhs.m_count <=> lhs.m_count;
	}
	template<typename Sentinel>
	friend constexpr auto operator<=>(counted_iterator const & lhs, counted_sentinel<Sentinel> const & rhs) {
		return lhs == rhs ? std::strong_ordering::equal : std::strong_ordering::less;
	}
	template<typename Sentinel>
	friend constexpr auto operator<=>(counted_sentinel<Sentinel> const & lhs, counted_iterator const & rhs) {
		return 0 <=> (rhs <=> lhs);
	}

	// TODO: Properly constrain this function
	template<bounded::integral Offset> requires(
		bounded::max_value<decltype(std::declval<Count>() - std::declval<Offset>())> >= bounded::constant<0>
	)
	friend constexpr auto operator+(counted_iterator it, Offset const offset) {
		auto const count = bounded::increase_min<0>(it.m_count - offset);
		return counted_iterator(std::move(it).m_it + offset, count);
	}
	friend constexpr auto operator-(counted_iterator const & lhs, counted_iterator const & rhs) {
		return lhs.m_count - rhs.m_count;
	}
	template<typename Sentinel>
	friend constexpr auto operator-(counted_sentinel<Sentinel> const & lhs, counted_iterator const & rhs) {
		return bounded::min(rhs.m_count, lhs.base() - rhs.m_it);
	}
	template<typename Sentinel>
	friend constexpr auto operator-(counted_iterator const & lhs, counted_sentinel<Sentinel> const & rhs) {
		return -(rhs - lhs);
	}
private:
	Iterator m_it;
	// TODO: Constrain maximum value with the difference type?
	Count m_count;
};

template<typename Iterator, typename Count> requires(bounded::max_value<Count> == bounded::constant<0>)
constexpr auto & operator++(counted_iterator<Iterator, Count> & it) {
	bounded::unreachable();
	return it;
}


// TODO: Handle range having a sentinel type
template<bounded::bounded_integer Count>
constexpr auto take(range auto && source, Count count_) {
	auto const count = bounded::integer<0, bounded::detail::builtin_max_value<Count>, typename Count::overflow_policy>(count_);
	return containers::range_view(
		counted_iterator(begin(BOUNDED_FORWARD(source)), count),
		counted_sentinel(end(BOUNDED_FORWARD(source)))
	);
}

} // namespace containers
