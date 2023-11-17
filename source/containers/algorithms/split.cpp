// Copyright David Stone 2023.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

module;

#include <operators/forward.hpp>

export module containers.algorithms.split;

import containers.algorithms.find;
import containers.begin_end;
export import containers.common_iterator_functions;
import containers.forward_range;
import containers.iter_difference_t;
import containers.range_view;
import containers.reference_or_value;
import containers.reference_wrapper;

import bounded;
import numeric_traits;
import std_module;

namespace containers {
using namespace bounded::literal;

struct split_sentinel {
};

template<typename Integer>
using one_wider = bounded::integer<
	bounded::normalize<numeric_traits::min_value<Integer> - 1_bi>,
	bounded::normalize<numeric_traits::max_value<Integer> + 1_bi>
>;

template<typename Iterator, typename Sentinel, typename Delimiter>
struct split_iterator {
	using difference_type = one_wider<iter_difference_t<Iterator>>;
	constexpr split_iterator(Iterator first, Sentinel last, Delimiter const & delimiter):
		m_first(first),
		m_last(last),
		m_delimiter(delimiter),
		m_empty_trailing_range(m_first == m_last),
		m_middle(find_from_first())
	{
	}

	constexpr auto operator*() const {
		return range_view(m_first, m_middle);
	}
	friend constexpr auto operator+(split_iterator lhs, bounded::constant_t<1>) {
		lhs.m_first = lhs.m_middle;
		if (lhs.m_first == lhs.m_last) {
			lhs.m_empty_trailing_range = false;
			return lhs;
		}
		++lhs.m_first; // skip the delimiter
		lhs.m_empty_trailing_range = lhs.m_first == lhs.m_last;
		lhs.m_middle = lhs.find_from_first();
		return lhs;
	}

	// It is undefined behavior to compare iterators into two different `split`
	// ranges
	friend constexpr auto operator<=>(split_iterator const & lhs, split_iterator const & rhs) requires bounded::ordered<Iterator> {
		if (auto cmp = lhs.m_first <=> rhs.m_first; cmp != 0) {
			return cmp;
		}
		return lhs.m_empty_trailing_range <=> rhs.m_empty_trailing_range;
	}
	friend constexpr auto operator==(split_iterator const & lhs, split_iterator const & rhs) -> bool {
		return lhs.m_first == rhs.m_first and lhs.m_empty_trailing_range == rhs.m_empty_trailing_range;
	}
	friend constexpr auto operator==(split_iterator const & lhs, split_sentinel) -> bool {
		return lhs.m_first == lhs.m_last and !lhs.m_empty_trailing_range;
	}
private:
	constexpr auto find_from_first() const -> Iterator {
		return containers::find(
			m_first,
			m_last,
			::containers::unwrap(m_delimiter)
		);
	}
	[[no_unique_address]] Iterator m_first;
	[[no_unique_address]] Sentinel m_last;
	[[no_unique_address]] reference_or_value<Delimiter> m_delimiter;
	[[no_unique_address]] bool m_empty_trailing_range;
	[[no_unique_address]] Iterator m_middle;
};

export template<forward_range Range, typename Delimiter>
struct split {
	static_assert(!std::is_array_v<std::remove_cvref_t<Range>>);
	constexpr split(Range && range, Delimiter && delimiter):
		m_range(OPERATORS_FORWARD(range)),
		m_delimiter(OPERATORS_FORWARD(delimiter))
	{
	}
	constexpr auto begin() const & {
		return split_iterator(
			containers::begin(m_range),
			containers::end(m_range),
			m_delimiter
		);
	}
	constexpr auto begin() & {
		return split_iterator(
			containers::begin(m_range),
			containers::end(m_range),
			m_delimiter
		);
	}
	constexpr auto begin() && {
		return split_iterator(
			containers::begin(std::move(m_range)),
			containers::end(std::move(m_range)),
			m_delimiter
		);
	}
	static constexpr auto end() {
		return split_sentinel();
	}
private:
	[[no_unique_address]] Range m_range;
	[[no_unique_address]] Delimiter m_delimiter;
};

template<typename Range, typename Delimiter>
split(Range &&, Delimiter &&) -> split<Range, Delimiter>;

} // namespace containers