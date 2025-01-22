// Copyright David Stone 2023.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

module;

#include <operators/forward.hpp>

export module containers.algorithms.chunk_by;

import containers.algorithms.adjacent;
import containers.algorithms.find;
import containers.back;
import containers.begin_end;
export import containers.common_iterator_functions;
import containers.forward_range;
import containers.iter_difference_t;
import containers.iter_reference_t;
import containers.stored_function;
import containers.subrange;

import bounded;
import numeric_traits;
import std_module;

namespace containers {
using namespace bounded::literal;

template<typename Iterator, typename Sentinel, typename BinaryPredicate>
struct chunk_by_iterator {
	using difference_type = iter_difference_t<Iterator>;
	constexpr chunk_by_iterator(Iterator first, Sentinel last, BinaryPredicate predicate):
		m_first(std::move(first)),
		m_last(last),
		m_predicate(predicate),
		m_middle(find_from_first())
	{
	}

	constexpr auto operator*() const {
		return subrange(m_first, m_middle);
	}
	friend constexpr auto operator+(chunk_by_iterator lhs, bounded::constant_t<1>) {
		lhs.m_first = lhs.m_middle;
		lhs.m_middle = lhs.find_from_first();
		return lhs;
	}

	// It is undefined behavior to compare iterators into two different
	// `chunk_by` ranges
	friend constexpr auto operator<=>(chunk_by_iterator const & lhs, chunk_by_iterator const & rhs) requires bounded::ordered<Iterator> {
		return lhs.m_first <=> rhs.m_first;
	}
	friend constexpr auto operator<=>(chunk_by_iterator const & lhs, std::default_sentinel_t) requires bounded::ordered<Iterator, Sentinel> {
		return lhs.m_first <=> lhs.m_last;
	}
	friend constexpr auto operator==(chunk_by_iterator const & lhs, chunk_by_iterator const & rhs) -> bool {
		return lhs.m_first == rhs.m_first;
	}
	friend constexpr auto operator==(chunk_by_iterator const & lhs, std::default_sentinel_t) -> bool {
		return lhs.m_first == lhs.m_last;
	}
private:
	using stored_predicate = stored_function<
		BinaryPredicate,
		iter_reference_t<Iterator>,
		iter_reference_t<Iterator>
	>;

	constexpr auto find_from_first() -> Iterator {
		auto it = containers::find_if(
			containers::adjacent(containers::subrange(m_first, m_last), 2_bi),
			[&](auto tuple) {
				return !m_predicate.get()(std::move(tuple)[0_bi], std::move(tuple)[1_bi]);
			}
		);
		return containers::back(it.base());
	}

	[[no_unique_address]] Iterator m_first;
	[[no_unique_address]] Sentinel m_last;
	[[no_unique_address]] stored_predicate m_predicate;
	[[no_unique_address]] Iterator m_middle;
};

export template<forward_range Range, typename BinaryPredicate>
struct chunk_by {
	static_assert(!std::is_array_v<std::remove_cvref_t<Range>>);
	constexpr chunk_by(Range && range, BinaryPredicate predicate):
		m_range(OPERATORS_FORWARD(range)),
		m_predicate(std::move(predicate))
	{
	}
	constexpr auto begin() const & {
		return chunk_by_iterator(
			containers::begin(m_range),
			containers::end(m_range),
			m_predicate
		);
	}
	constexpr auto begin() & {
		return chunk_by_iterator(
			containers::begin(m_range),
			containers::end(m_range),
			m_predicate
		);
	}
	constexpr auto begin() && {
		return chunk_by_iterator(
			containers::begin(std::move(m_range)),
			containers::end(std::move(m_range)),
			m_predicate
		);
	}
	static constexpr auto end() {
		return std::default_sentinel;
	}
private:
	[[no_unique_address]] Range m_range;
	[[no_unique_address]] BinaryPredicate m_predicate;
};

template<typename Range, typename BinaryPredicate>
chunk_by(Range &&, BinaryPredicate) -> chunk_by<Range, BinaryPredicate>;

} // namespace containers