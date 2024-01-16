// Copyright David Stone 2023.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

module;

#include <operators/forward.hpp>

export module containers.algorithms.join;

import containers.begin_end;
export import containers.common_iterator_functions;
import containers.is_empty;
import containers.iter_difference_t;
import containers.iter_reference_t;
import containers.iter_value_t;
import containers.iterator_t;
import containers.range;
import containers.range_reference_t;
import containers.range_size_t;
import containers.sentinel_t;

import bounded;
import std_module;
import tv;

namespace containers {
using namespace bounded::literal;

template<typename Range>
struct inner_range {
	inner_range() = default;
	inner_range(inner_range &&) = default;
	inner_range(inner_range const &) = delete;
	auto operator=(inner_range &&) & -> inner_range & = default;
	auto operator=(inner_range const &) & -> inner_range & = delete;

	constexpr auto set(auto const & outer_it) & -> void {
		m_range.emplace([&] { return *outer_it; });
		m_it = containers::begin(std::move(*m_range));
	}

	constexpr auto begin() const -> decltype(auto) {
		return m_it;
	}
	// Requires that `set` has already been called
	constexpr auto end() const -> decltype(auto) {
		return containers::end(*m_range);
	}

	constexpr auto advance() & -> void {
		++m_it;
	}

private:
	tv::optional<Range> m_range{};
	iterator_t<Range> m_it{};
};

template<typename Range>
struct inner_range_iterator {
	constexpr inner_range_iterator(inner_range<Range> & range):
		m_range(range)
	{
	}
	constexpr auto set(auto const & outer_it) const -> void {
		m_range.get().set(outer_it);
	}

	constexpr auto begin() const -> decltype(auto) {
		return m_range.get().begin();
	}
	// Requires that `set` has already been called
	constexpr auto end() const -> decltype(auto) {
		return m_range.get().end();
	}

	constexpr auto advance() const -> void {
		m_range.get().advance();
	}

private:
	std::reference_wrapper<inner_range<Range>> m_range;
};

template<typename Range> requires std::is_reference_v<Range>
struct inner_range<Range> {
};

template<typename Range> requires std::is_reference_v<Range>
struct inner_range_iterator<Range> {
	constexpr inner_range_iterator(inner_range<Range>):
		m_it(),
		m_last()
	{
	}
	constexpr auto set(auto const & outer_it) & -> void {
		auto && range = *outer_it;
		m_it = containers::begin(range);
		m_last = containers::end(range);
	}

	constexpr auto begin() const -> iterator_t<Range> const & {
		return m_it;
	}
	constexpr auto end() const -> sentinel_t<Range> const & {
		return m_last;
	}

	constexpr auto advance() & -> void {
		++m_it;
	}

private:
	iterator_t<Range> m_it;
	sentinel_t<Range> m_last;
};

// TODO: It's possible to more efficiently add and subtract iterators
template<typename Iterator, typename Sentinel>
struct join_iterator {
	using difference_type = decltype(
		bounded::declval<iter_difference_t<Iterator>>() *
		bounded::declval<range_size_t<iter_value_t<Iterator>>>()
	);
	explicit constexpr join_iterator(
		Iterator it,
		Sentinel last,
		inner_range<iter_reference_t<Iterator>> & inner
	):
		m_outer_it(std::move(it)),
		m_outer_last(std::move(last)),
		m_inner(inner)
	{
		if (m_outer_it != m_outer_last) {
			m_inner.set(m_outer_it);
			normalize();
		}
	}

	constexpr auto operator*() const -> decltype(auto) {
		return *m_inner.begin();
	}
	friend constexpr auto operator+(join_iterator lhs, bounded::constant_t<1>) {
		lhs.m_inner.advance();
		lhs.normalize();
		return lhs;
	}
	friend constexpr auto operator==(join_iterator const & lhs, join_iterator const & rhs) -> bool {
		return
			lhs.m_outer_it == rhs.m_outer_it and
			lhs.m_inner.begin() == rhs.m_inner.begin();
	}
	friend constexpr auto operator==(join_iterator const & lhs, std::default_sentinel_t) -> bool {
		return lhs.m_outer_it == lhs.m_outer_last;
	}

private:
	constexpr auto normalize() & -> void {
		while (containers::is_empty(m_inner)) {
			++m_outer_it;
			if (m_outer_it == m_outer_last) {
				return;
			}
			m_inner.set(m_outer_it);
		}
	}

	Iterator m_outer_it;
	Sentinel m_outer_last;
	inner_range_iterator<iter_reference_t<Iterator>> m_inner;
};

export template<range Range>
struct join {
	explicit constexpr join(Range && range):
		m_range(OPERATORS_FORWARD(range))
	{
	}
	constexpr auto begin() const & requires std::is_reference_v<range_reference_t<Range>> {
		return join_iterator(
			containers::begin(m_range),
			containers::end(m_range),
			m_inner
		);
	}
	constexpr auto begin() & requires std::is_reference_v<range_reference_t<Range>> {
		return join_iterator(
			containers::begin(m_range),
			containers::end(m_range),
			m_inner
		);
	}
	constexpr auto begin() && {
		return join_iterator(
			containers::begin(std::move(m_range)),
			containers::end(std::move(m_range)),
			m_inner
		);
	}
	// TODO: Support bidirectional?
	static constexpr auto end() {
		return std::default_sentinel;
	}
private:
	[[no_unique_address]] Range m_range;
	[[no_unique_address]] inner_range<range_reference_t<Range>> m_inner;
};

} // namespace containers