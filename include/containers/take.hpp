// Copyright David Stone 2019.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/begin_end.hpp>
#include <containers/common_iterator_functions.hpp>
#include <containers/iter_difference_t.hpp>
#include <containers/iter_value_t.hpp>
#include <containers/range_view.hpp>

#include <bounded/clamp.hpp>
#include <bounded/integer.hpp>
#include <bounded/unreachable.hpp>

#include <operators/forward.hpp>
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

namespace detail {

template<typename Difference, typename Count>
using counted_offset_type = bounded::integer<
	0,
	bounded::normalize<bounded::min(bounded::integer(bounded::max_value<Difference>), bounded::max_value<Count>).value()>
>;

} // namespace detail

template<typename Iterator, typename Count>
struct counted_iterator {
private:
	using offset_type = detail::counted_offset_type<iter_difference_t<Iterator>, Count>;
public:
	using iterator_category = typename std::iterator_traits<Iterator>::iterator_category;
	using value_type = iter_value_t<Iterator>;
	using difference_type = decltype(std::declval<offset_type>() - std::declval<offset_type>());
	using pointer = typename std::iterator_traits<Iterator>::pointer;
	using reference = typename std::iterator_traits<Iterator>::reference;

	constexpr explicit counted_iterator(Iterator it, Count count):
		m_it(std::move(it)),
		m_count(bounded::clamp(count, bounded::min_value<offset_type>, bounded::max_value<offset_type>))
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
		bounded::max_value<decltype(std::declval<offset_type>() - std::declval<Offset>())> >= bounded::constant<0> and
		requires(Iterator it, Offset offset) { it + offset; }
	)
	friend constexpr auto operator+(counted_iterator it, Offset const offset) {
		return counted_iterator(
			std::move(it).m_it + offset,
			static_cast<offset_type>(it.m_count - offset)
		);
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
	offset_type m_count;
};

template<typename Iterator, typename Count> requires(bounded::max_value<iter_difference_t<counted_iterator<Iterator, Count>>> == bounded::constant<0>)
constexpr auto & operator++(counted_iterator<Iterator, Count> & it) {
	bounded::unreachable();
	return it;
}

template<bounded::bounded_integer Count>
constexpr auto take(range auto && source, Count count_) {
	auto const count = bounded::integer<0, bounded::detail::builtin_max_value<Count>>(count_);
	return containers::range_view(
		counted_iterator(containers::begin(OPERATORS_FORWARD(source)), count),
		counted_sentinel(containers::end(OPERATORS_FORWARD(source)))
	);
}

} // namespace containers
