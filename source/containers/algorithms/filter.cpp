// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

module;

#include <bounded/assert.hpp>

#include <operators/forward.hpp>
#include <operators/returns.hpp>

export module containers.algorithms.filter;

import containers.algorithms.advance;
import containers.algorithms.compare;
import containers.algorithms.find;
import containers.array;
import containers.begin_end;
import containers.default_adapt_traits;
import containers.default_begin_end_size;
import containers.is_iterator;
import containers.is_iterator_sentinel;
import containers.iterator_adapter;
import containers.range_value_t;
import containers.reference_wrapper;
import containers.sentinel_t;

import bounded;
import std_module;

using namespace bounded::literal;

namespace containers {

// TODO: use a custom begin function
template<typename Sentinel, typename UnaryPredicate>
struct filter_iterator_traits : default_begin_end_size, default_dereference, default_compare {
	constexpr filter_iterator_traits(Sentinel last, UnaryPredicate condition):
		m_sentinel(std::move(last)),
		m_predicate(std::move(condition))
	{
	}

	constexpr auto sentinel() const -> Sentinel {
		return m_sentinel;
	}
	constexpr auto predicate() const -> UnaryPredicate const & {
		return m_predicate;
	}

	template<iterator Iterator>
	constexpr auto add(Iterator it, bounded::constant_t<1>) const -> Iterator {
		BOUNDED_ASSERT(it != m_sentinel);
		return containers::find_if(
			containers::next(std::move(it)),
			m_sentinel,
			m_predicate
		);
	}
private:
	[[no_unique_address]] Sentinel m_sentinel;
	[[no_unique_address]] UnaryPredicate m_predicate;
};

template<typename T>
constexpr auto is_filter_iterator_traits_impl = false;

template<typename Sentinel, typename UnaryPredicate>
constexpr auto is_filter_iterator_traits_impl<filter_iterator_traits<Sentinel, UnaryPredicate>> = true;

template<typename T>
constexpr auto is_filter_iterator_traits_impl<std::reference_wrapper<T>> = is_filter_iterator_traits_impl<T>;

template<typename T>
constexpr auto is_filter_iterator_traits_impl<T const> = is_filter_iterator_traits_impl<T>;

// TODO: Change name to something without "is"
template<typename T>
concept is_filter_iterator_traits = is_filter_iterator_traits_impl<T>;

struct filter_iterator_sentinel {
	template<iterator Iterator, is_filter_iterator_traits Traits>
	friend constexpr auto operator<=>(adapt_iterator<Iterator, Traits> const & lhs, filter_iterator_sentinel) OPERATORS_RETURNS(
		lhs.traits().compare(lhs.base(), lhs.traits().sentinel())
	)

	template<iterator Iterator, is_filter_iterator_traits Traits>
	friend constexpr auto operator==(adapt_iterator<Iterator, Traits> const & lhs, filter_iterator_sentinel) -> bool {
		return lhs.traits().equal(lhs.base(), lhs.traits().sentinel());
	}
};

constexpr auto filter_iterator_impl(iterator auto first, is_filter_iterator_traits auto traits) {
	return containers::adapt_iterator(
		containers::find_if(
			std::move(first),
			::containers::unwrap(traits).sentinel(),
			::containers::unwrap(traits).predicate()
		),
		traits
	);
}

export template<typename Range, typename UnaryPredicate>
struct filter {
	constexpr filter(Range && range, UnaryPredicate predicate):
		m_range(OPERATORS_FORWARD(range)),
		m_traits(::containers::end(std::as_const(m_range)), std::move(predicate))
	{
	}
	
	constexpr auto begin() const & {
		return ::containers::filter_iterator_impl(::containers::begin(m_range), std::reference_wrapper(m_traits));
	}
	constexpr auto begin() & {
		return ::containers::filter_iterator_impl(::containers::begin(m_range), std::reference_wrapper(m_traits));
	}
	constexpr auto begin() && {
		return ::containers::filter_iterator_impl(::containers::begin(std::move(*this).m_range), std::reference_wrapper(m_traits));
	}
	static constexpr auto end() {
		return filter_iterator_sentinel();
	}
	
private:
	Range m_range;
	filter_iterator_traits<sentinel_t<Range const &>, UnaryPredicate> m_traits;
};

template<typename Range, typename UnaryPredicate>
filter(Range &&, UnaryPredicate) -> filter<Range, UnaryPredicate>;

} // namespace containers

constexpr auto check_filter() {
	constexpr auto source = containers::array{1_bi, 2_bi, 3_bi, 2_bi, 4_bi, 5_bi, 6_bi, 8_bi};
	constexpr auto expected = containers::array{2_bi, 2_bi, 4_bi, 6_bi, 8_bi};
	auto const filtered = containers::filter(source, [](auto const integer) { return integer % 2_bi == 0_bi; });
	return containers::equal(filtered, expected);
}

static_assert(check_filter());
