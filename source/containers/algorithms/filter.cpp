// Copyright David Stone 2023.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

module;

#include <bounded/assert.hpp>

#include <operators/forward.hpp>

export module containers.algorithms.filter;

import containers.algorithms.advance;
import containers.algorithms.find;

import containers.adapt;
import containers.begin_end;
import containers.default_adapt_traits;
import containers.iterator;
import containers.range;
import containers.reference_or_value;

import bounded;
import std_module;

namespace containers {

using namespace bounded::literal;

template<typename Sentinel, typename UnaryPredicate>
struct filter_iterator_traits : default_dereference, default_compare {
	constexpr filter_iterator_traits(Sentinel last, UnaryPredicate const & predicate):
		m_sentinel(std::move(last)),
		m_predicate(predicate)
	{
	}

	template<iterator Iterator>
	constexpr auto add(Iterator it, bounded::constant_t<1>) const -> Iterator {
		BOUNDED_ASSERT(it != m_sentinel);
		return containers::find_if(
			containers::next(std::move(it)),
			m_sentinel,
			m_predicate.get()
		);
	}
private:
	[[no_unique_address]] Sentinel m_sentinel;
	[[no_unique_address]] reference_or_value<UnaryPredicate> m_predicate;
};

template<typename UnaryPredicate>
struct filter_traits {
	constexpr filter_traits(UnaryPredicate condition):
		m_predicate(std::move(condition))
	{
	}

	constexpr auto get_begin(range auto && base) const {
		return containers::find_if(
			containers::begin(OPERATORS_FORWARD(base)),
			containers::end(base),
			m_predicate
		);
	}

	static constexpr auto get_end(range auto && base) {
		return containers::end(base);
	}

	constexpr auto iterator_traits(range auto & base) const {
		return filter_iterator_traits(
			containers::end(base),
			m_predicate
		);
	}

private:
	[[no_unique_address]] UnaryPredicate m_predicate;
};

export constexpr auto filter(range auto && source, auto predicate) -> range auto {
	return adapt(
		OPERATORS_FORWARD(source),
		filter_traits(std::move(predicate))
	);
}

} // namespace containers
