// Copyright David Stone 2023.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

module;

#include <operators/forward.hpp>

export module containers.algorithms.join;

import containers.begin_end;
export import containers.common_iterator_functions;
import containers.iter_difference_t;
import containers.iter_value_t;
import containers.iterator_t;
import containers.range;
import containers.range_size_t;
import containers.sentinel_t;

import bounded;
import std_module;

namespace containers {
using namespace bounded::literal;

// TODO: Protect against returning a temporary container that would cause the
// iterators to dangle
// TODO: It's possible to more efficiently add and subtract iterators
template<typename Iterator, typename Sentinel>
struct join_iterator {
	using difference_type = decltype(
		bounded::declval<iter_difference_t<Iterator>>() *
		bounded::declval<range_size_t<iter_value_t<Iterator>>>()
	);
	explicit constexpr join_iterator(Iterator it, Sentinel last):
		m_outer_it(std::move(it)),
		m_outer_last(std::move(last)),
		m_inner_it(),
		m_inner_last()
	{
		if (m_outer_it != m_outer_last) {
			auto && range = *m_outer_it;
			m_inner_it = containers::begin(range);
			m_inner_last = containers::end(range);
			normalize();
		}
	}

	constexpr auto operator*() const -> decltype(auto) {
		return *m_inner_it;
	}
	friend constexpr auto operator+(join_iterator lhs, bounded::constant_t<1>) {
		++lhs.m_inner_it;
		lhs.normalize();
		return lhs;
	}
	friend constexpr auto operator==(join_iterator const & lhs, join_iterator const & rhs) -> bool {
		return lhs.m_outer_it == rhs.m_outer_it and lhs.m_inner_it == rhs.m_inner_it;
	}
	friend constexpr auto operator==(join_iterator const & lhs, std::default_sentinel_t) -> bool {
		return lhs.m_outer_it == lhs.m_outer_last;
	}

private:
	constexpr auto normalize() & -> void {
		while (m_inner_it == m_inner_last) {
			++m_outer_it;
			if (m_outer_it == m_outer_last) {
				return;
			}
			auto && inner = *m_outer_it;
			m_inner_it = containers::begin(inner);
			m_inner_last = containers::end(inner);
		}
	}

	Iterator m_outer_it;
	Sentinel m_outer_last;
	iterator_t<iter_value_t<Iterator>> m_inner_it;
	sentinel_t<iter_value_t<Iterator>> m_inner_last;
};

export template<range Range>
struct join {
	explicit constexpr join(Range && range):
		m_range(OPERATORS_FORWARD(range))
	{
	}
	constexpr auto begin() const & {
		return join_iterator(
			containers::begin(m_range),
			containers::end(m_range)
		);
	}
	constexpr auto begin() & {
		return join_iterator(
			containers::begin(m_range),
			containers::end(m_range)
		);
	}
	constexpr auto begin() && {
		return join_iterator(
			containers::begin(std::move(m_range)),
			containers::end(std::move(m_range))
		);
	}
	// TODO: Support bidirectional?
	static constexpr auto end() {
		return std::default_sentinel;
	}
private:
	[[no_unique_address]] Range m_range;
};

} // namespace containers