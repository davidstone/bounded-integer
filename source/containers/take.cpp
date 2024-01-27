// Copyright David Stone 2019.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

module;

#include <bounded/assert.hpp>

#include <operators/arrow.hpp>
#include <operators/forward.hpp>

export module containers.take;

import containers.begin_end;
import containers.forward_random_access_iterator;
import containers.iter_difference_t;
import containers.iterator_addable;
import containers.iterator_category_base;
import containers.iterator_t;
import containers.random_access_sentinel_for;
import containers.range;
import containers.range_size_t;
import containers.range_view;
import containers.sentinel_t;
import containers.size;
import containers.sized_range;

import bounded;
import numeric_traits;
import std_module;

export import containers.common_iterator_functions;

using namespace bounded::literal;

namespace containers {

enum class extra_elements_policy {
	drop, exception
};

template<typename Sentinel, extra_elements_policy>
struct counted_sentinel {
	constexpr explicit counted_sentinel(Sentinel sentinel):
		m_sentinel(std::move(sentinel))
	{
	}

	constexpr auto base() const {
		return m_sentinel;
	}

private:
	[[no_unique_address]] Sentinel m_sentinel;
};

template<typename Iterator, typename Count>
struct counted_iterator : iterator_category_base<Iterator> {
	using difference_type = decltype(bounded::declval<Count>() - bounded::declval<Count>());

	constexpr explicit counted_iterator(Iterator it, Count count):
		m_it(std::move(it)),
		m_count(count)
	{
	}

	constexpr auto base() const & -> Iterator const & {
		return m_it;
	}
	constexpr auto base() && -> Iterator && {
		return m_it;
	}

	constexpr decltype(auto) operator*() const {
		return *m_it;
	}
	OPERATORS_ARROW_DEFINITIONS

	friend constexpr auto operator==(counted_iterator const & lhs, counted_iterator const & rhs) -> bool {
		return rhs.m_count == lhs.m_count;
	}
	template<typename Sentinel, extra_elements_policy policy>
	friend constexpr auto operator==(counted_iterator const & lhs, counted_sentinel<Sentinel, policy> const & rhs) -> bool {
		if constexpr (policy == extra_elements_policy::drop) {
			return lhs.m_count == 0_bi or lhs.m_it == rhs.base();
		} else {
			if (lhs.m_it == rhs.base()) {
				return true;
			}
			if (lhs.m_count == 0_bi) {
				throw std::runtime_error("Did not take all elements of range");
			}
			return false;
		}
	}

	// A smaller count means you are further along
	friend constexpr auto operator<=>(counted_iterator const & lhs, counted_iterator const & rhs) {
		return rhs.m_count <=> lhs.m_count;
	}
	template<typename Sentinel, extra_elements_policy policy>
	friend constexpr auto operator<=>(counted_iterator const & lhs, counted_sentinel<Sentinel, policy> const & rhs) {
		return lhs == rhs ? std::strong_ordering::equal : std::strong_ordering::less;
	}

	template<typename Offset> requires(
		numeric_traits::max_value<decltype(bounded::declval<Count>() - bounded::declval<Offset>())> >= 0_bi and
		iterator_addable<Iterator, Offset>
	)
	friend constexpr auto operator+(counted_iterator it, Offset const offset) {
		return counted_iterator(
			std::move(it).m_it + offset,
			::bounded::assume_in_range<Count>(it.m_count - offset)
		);
	}
	friend constexpr auto operator-(counted_iterator const & lhs, counted_iterator const & rhs) {
		return rhs.m_count - lhs.m_count;
	}
	template<random_access_sentinel_for<Iterator> Sentinel, extra_elements_policy policy>
	friend constexpr auto operator-(counted_sentinel<Sentinel, policy> const & lhs, counted_iterator const & rhs) {
		static_assert(std::same_as<
			decltype(rhs.m_count),
			decltype(bounded::min(rhs.m_count, lhs.base() - rhs.m_it))
		>);
		BOUNDED_ASSERT(rhs.m_count == lhs.base() - rhs.m_it);
		return rhs.m_count;
	}
	template<random_access_sentinel_for<Iterator> Sentinel, extra_elements_policy policy>
	friend constexpr auto operator-(counted_iterator const & lhs, counted_sentinel<Sentinel, policy> const & rhs) {
		return -(rhs - lhs);
	}

	friend auto & operator++(counted_iterator & it) requires(numeric_traits::max_value<difference_type> == 0_bi) {
		std::unreachable();
		return it;
	}

private:
	[[no_unique_address]] Iterator m_it;
	[[no_unique_address]] Count m_count;
};

template<typename Iterator, typename Count>
struct random_access_counted_iterator {
	using difference_type = decltype(bounded::declval<Count>() - bounded::declval<Count>());

	constexpr explicit random_access_counted_iterator(Iterator it):
		m_it(std::move(it))
	{
	}

	constexpr auto base() const -> Iterator {
		return m_it;
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
		return ::bounded::assume_in_range<difference_type>(std::move(lhs).m_it - std::move(rhs).m_it);
	}

	friend auto & operator++(random_access_counted_iterator & it) requires(numeric_traits::max_value<difference_type> == 0_bi) {
		std::unreachable();
		return it;
	}

private:
	[[no_unique_address]] Iterator m_it;
};

template<extra_elements_policy policy, range Range, bounded::bounded_integer Count> requires(numeric_traits::min_value<Count> >= 0_bi)
constexpr auto take_impl(Range && source, Count const count) {
	using iterator = iterator_t<Range>;
	using count_type = bounded::integer<
		0,
		bounded::normalize<bounded::min(
			bounded::constant<bounded::builtin_max_value<iter_difference_t<iterator>>>,
			bounded::constant<bounded::builtin_max_value<Count>>
		)>
	>;

	if constexpr (sized_range<Range> and policy == extra_elements_policy::exception) {
		auto const source_size = containers::size(source);
		if (source_size > count) {
			throw std::runtime_error("Did not take all elements of range");
		}
		return containers::range_view(OPERATORS_FORWARD(source));
	} else if constexpr (sized_range<Range> and forward_random_access_iterator<iterator>) {
		auto const true_count = bounded::min(containers::size(source), count);
		auto first = random_access_counted_iterator<iterator, count_type>(containers::begin(OPERATORS_FORWARD(source)));
		return containers::range_view(first, true_count);
	} else {
		auto const used_count = count_type(bounded::min(count, numeric_traits::max_value<count_type>));
		return containers::range_view(
			counted_iterator(containers::begin(OPERATORS_FORWARD(source)), used_count),
			counted_sentinel<sentinel_t<Range>, policy>(containers::end(OPERATORS_FORWARD(source)))
		);
	}
}

export template<range Range, bounded::bounded_integer Count> requires(numeric_traits::min_value<Count> >= 0_bi)
constexpr auto take(Range && source, Count const count) {
	return ::containers::take_impl<extra_elements_policy::drop>(OPERATORS_FORWARD(source), count);
}

export template<range Range, bounded::bounded_integer Count> requires(0_bi <= numeric_traits::min_value<Count> and numeric_traits::min_value<range_size_t<Range>> <= numeric_traits::max_value<Count>)
constexpr auto check_size_not_greater_than(Range && source, Count const count) {
	return ::containers::take_impl<extra_elements_policy::exception>(OPERATORS_FORWARD(source), count);
}

} // namespace containers