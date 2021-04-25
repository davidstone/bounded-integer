// Copyright David Stone 2019.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/begin_end.hpp>
#include <containers/common_iterator_functions.hpp>
#include <containers/iter_difference_t.hpp>
#include <containers/iterator_t.hpp>
#include <containers/iter_value_t.hpp>
#include <containers/range_view.hpp>
#include <containers/size.hpp>

#include <bounded/clamp.hpp>
#include <bounded/integer.hpp>
#include <bounded/unreachable.hpp>

#include <operators/forward.hpp>
#include <operators/operators.hpp>

#include <iterator>
#include <utility>

namespace containers {
namespace detail {

using namespace bounded::literal;

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
	using value_type = iter_value_t<Iterator>;
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

	friend constexpr auto operator==(counted_iterator const & lhs, counted_iterator const & rhs) -> bool {
		return rhs.m_count == lhs.m_count;
	}
	template<typename Sentinel>
	friend constexpr auto operator==(counted_iterator const & lhs, counted_sentinel<Sentinel> const & rhs) -> bool {
		return lhs.m_count == 0_bi or lhs.m_it == rhs.base();
	}

	// A smaller count means you are further along
	friend constexpr auto operator<=>(counted_iterator const & lhs, counted_iterator const & rhs) {
		return rhs.m_count <=> lhs.m_count;
	}
	template<typename Sentinel>
	friend constexpr auto operator<=>(counted_iterator const & lhs, counted_sentinel<Sentinel> const & rhs) {
		return lhs == rhs ? std::strong_ordering::equal : std::strong_ordering::less;
	}

	template<typename Offset> requires(
		bounded::max_value<decltype(std::declval<Count>() - std::declval<Offset>())> >= 0_bi and
		requires(Iterator it, Offset offset) { it + offset; }
	)
	friend constexpr auto operator+(counted_iterator it, Offset const offset) {
		return counted_iterator(
			std::move(it).m_it + offset,
			static_cast<Count>(it.m_count - offset)
		);
	}
	friend constexpr auto operator-(counted_iterator const & lhs, counted_iterator const & rhs) {
		return rhs.m_count - lhs.m_count;
	}
	template<random_access_sentinel_for<Iterator> Sentinel>
	friend constexpr auto operator-(counted_sentinel<Sentinel> const & lhs, counted_iterator const & rhs) {
		return bounded::min(rhs.m_count, lhs.base() - rhs.m_it);
	}
	template<random_access_sentinel_for<Iterator> Sentinel>
	friend constexpr auto operator-(counted_iterator const & lhs, counted_sentinel<Sentinel> const & rhs) {
		return -(rhs - lhs);
	}

	friend auto & operator++(counted_iterator & it) requires(bounded::max_value<difference_type> == 0_bi) {
		bounded::unreachable();
		return it;
	}

private:
	Iterator m_it;
	Count m_count;
};

template<typename Iterator, typename Count>
struct random_access_counted_iterator {
	using iterator_category = typename std::iterator_traits<Iterator>::iterator_category;
	using value_type = iter_value_t<Iterator>;
	using difference_type = decltype(std::declval<Count>() - std::declval<Count>());
	using pointer = typename std::iterator_traits<Iterator>::pointer;
	using reference = typename std::iterator_traits<Iterator>::reference;

	constexpr explicit random_access_counted_iterator(Iterator it):
		m_it(std::move(it))
	{
	}

	constexpr decltype(auto) operator*() const {
		return *m_it;
	}
	OPERATORS_ARROW_DEFINITIONS

	friend auto operator<=>(random_access_counted_iterator const &, random_access_counted_iterator const &) = default;

	friend constexpr auto operator+(random_access_counted_iterator it, difference_type const offset) {
		return random_access_counted_iterator(std::move(it).m_it + offset);
	}
	friend constexpr auto operator-(random_access_counted_iterator lhs, random_access_counted_iterator rhs) {
		return static_cast<difference_type>(std::move(lhs).m_it - std::move(rhs).m_it);
	}

	friend auto & operator++(random_access_counted_iterator & it) requires(bounded::max_value<difference_type> == 0_bi) {
		bounded::unreachable();
		return it;
	}

private:
	Iterator m_it;
};

} // namespace detail

template<range Range, bounded::bounded_integer Count> requires(bounded::min_value<Count> >= 0_bi)
constexpr auto take(Range && source, Count const count) {
	using iterator = iterator_t<Range>;
	using count_type = bounded::integer<
		0,
		bounded::normalize<bounded::min(
			bounded::constant<bounded::builtin_max_value<iter_difference_t<iterator>>>,
			bounded::constant<bounded::builtin_max_value<Count>>
		)>
	>;

	if constexpr (sized_range<Range> and forward_random_access_iterator<iterator>) {
		auto const true_count = bounded::min(containers::size(source), count);
		auto first = detail::random_access_counted_iterator<iterator, count_type>(containers::begin(OPERATORS_FORWARD(source)));
		return containers::range_view(first, first + true_count);
	} else {
		auto const upper_bound_count = count_type(bounded::min(count, bounded::max_value<count_type>));
		return containers::range_view(
			detail::counted_iterator(containers::begin(OPERATORS_FORWARD(source)), upper_bound_count),
			detail::counted_sentinel(containers::end(OPERATORS_FORWARD(source)))
		);
	}
}

} // namespace containers
