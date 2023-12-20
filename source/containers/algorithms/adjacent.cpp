// Copyright David Stone 2023.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

module;

#include <bounded/conditional.hpp>

#include <operators/forward.hpp>

export module containers.algorithms.adjacent;

import containers.algorithms.advance;
import containers.array;
import containers.begin_end;
export import containers.common_iterator_functions;
import containers.forward_range;
import containers.front_back;
import containers.iter_difference_t;
import containers.size;
import containers.sized_range;

import bounded;
import numeric_traits;
import std_module;
import tv;

namespace containers {
using namespace bounded::literal;

template<typename Sentinel>
struct adjacent_sentinel {
	explicit constexpr adjacent_sentinel(Sentinel base_):
		m_base(base_)
	{
	}
	constexpr auto base() const -> Sentinel {
		return m_base;
	}
private:
	Sentinel m_base;
};

template<typename Difference, auto amount>
struct decrease_by_impl {
	static constexpr auto value = bounded::max(numeric_traits::max_value<Difference> - amount, 0_bi);
	using type = bounded::integer<bounded::normalize<-value>, bounded::normalize<value>>;
};

template<typename Difference, auto amount>
using decrease_by = typename decrease_by_impl<Difference, amount>::type;

template<typename Iterator, bounded::bounded_integer auto group_size> requires(group_size > 0_bi)
struct adjacent_iterator {
	using difference_type = decrease_by<iter_difference_t<Iterator>, group_size>;

	adjacent_iterator() = default;
	template<typename Sentinel>
	constexpr adjacent_iterator(
		Iterator it,
		Sentinel const last,
		bounded::constant_t<bounded::normalize<group_size>>)
	{
		for (auto & element : m_its) {
			element = it;
			if (it != last) {
				++it;
			}
		}
	}
	constexpr auto operator*() const {
		return [&]<std::size_t... indexes>(std::index_sequence<indexes...>) {
			return tv::tuple<
				decltype(*m_its[bounded::constant<indexes>])...
			>(
				*m_its[bounded::constant<indexes>]...
			);
		}(bounded::make_index_sequence(group_size));
	}
	friend constexpr auto operator+(adjacent_iterator lhs, bounded::bounded_integer auto const offset) -> adjacent_iterator {
		for (auto & it : lhs.m_its) {
			it += offset;
		}
		return lhs;
	}
	// Some iterators are expensive to increment. We can optimize the case where
	// we just want to increment everything by one to avoid incrementing
	// arbitrary iterators more than once.
	friend constexpr auto operator+(adjacent_iterator lhs, bounded::constant_t<1>) -> adjacent_iterator {
		auto const last = containers::prev(containers::end(lhs.m_its));
		auto it = containers::begin(lhs.m_its);
		while (it != last) {
			auto const next_it = containers::next(it);
			*it = *next_it;
			it = next_it;
		}
		++*it;
		return lhs;
	}

	friend constexpr auto operator-(adjacent_iterator const lhs, adjacent_iterator const rhs) -> difference_type {
		return bounded::assume_in_range<difference_type>(containers::back(lhs.m_its) - containers::back(rhs.m_its));
	}
	template<typename Sentinel>
	friend constexpr auto operator-(adjacent_sentinel<Sentinel> const lhs, adjacent_iterator const rhs) -> difference_type {
		return bounded::assume_in_range<difference_type>(lhs.base() - containers::back(rhs.m_its));
	}

	friend constexpr auto operator<=>(adjacent_iterator const lhs, adjacent_iterator const rhs) {
		return containers::back(lhs.m_its) <=> containers::back(rhs.m_its);
	}
	template<typename Sentinel>
	friend constexpr auto operator<=>(adjacent_iterator const lhs, adjacent_sentinel<Sentinel> const rhs) {
		return containers::back(lhs.m_its) <=> rhs.base();
	}

	friend constexpr auto operator==(adjacent_iterator const lhs, adjacent_iterator const rhs) -> bool {
		return containers::back(lhs.m_its) == containers::back(rhs.m_its);
	}
	template<typename Sentinel>
	friend constexpr auto operator==(adjacent_iterator const lhs, adjacent_sentinel<Sentinel> const rhs) -> bool {
		return containers::back(lhs.m_its) == rhs.base();
	}

private:
	containers::array<Iterator, group_size> m_its;
};

template<typename Iterator, typename Sentinel, auto group_size>
adjacent_iterator(Iterator, Sentinel, bounded::constant_t<group_size>) -> adjacent_iterator<Iterator, bounded::constant<group_size>>;

export template<forward_range Range, bounded::bounded_integer auto group_size>
struct adjacent {
	constexpr explicit adjacent(Range && r, bounded::constant_t<bounded::normalize<group_size>>):
		m_range(OPERATORS_FORWARD(r))
	{
	}
	constexpr auto begin() const & {
		return adjacent_iterator(
			containers::begin(m_range),
			containers::end(m_range),
			group_size
		);
	}
	constexpr auto begin() & {
		return adjacent_iterator(
			containers::begin(m_range),
			containers::end(m_range),
			group_size
		);
	}
	constexpr auto begin() && {
		return adjacent_iterator(
			containers::begin(OPERATORS_FORWARD(m_range)),
			containers::end(m_range),
			group_size
		);
	}
	constexpr auto end() const {
		// TODO: Support stronger iterator categories
		return adjacent_sentinel(containers::end(m_range));
	}
	constexpr auto size() const requires sized_range<Range> {
		auto const range_size = containers::size(m_range);
		return BOUNDED_CONDITIONAL(
			range_size < group_size,
			0_bi,
			range_size - group_size + 1_bi
		);
	}

private:
	Range m_range;
};

template<typename Range, auto group_size>
adjacent(Range &&, bounded::constant_t<group_size>) -> adjacent<Range, bounded::constant<group_size>>;

} // namespace containers