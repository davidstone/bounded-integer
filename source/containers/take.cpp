// Copyright David Stone 2019.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

module;

#include <compare>
#include <string>

#include <bounded/assert.hpp>
#include <bounded/arithmetic/common_arithmetic.hpp>

#include <operators/arrow.hpp>
#include <operators/forward.hpp>

export module containers.take;

import containers.algorithms.compare;
import containers.algorithms.concatenate;
import containers.addable_subtractable;
import containers.array;
import containers.begin_end;
import containers.is_iterator_sentinel;
import containers.is_range;
import containers.iter_difference_t;
import containers.iterator_category_base;
import containers.iter_value_t;
import containers.iterator_t;
import containers.range_view;
import containers.size;
import containers.to_any_string;

import bounded;
import numeric_traits;
import operators;
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
	Sentinel m_sentinel;
};

template<typename Iterator, typename Count>
struct counted_iterator : iterator_category_base<Iterator> {
	using difference_type = decltype(bounded::declval<Count>() - bounded::declval<Count>());

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
		addable<Iterator, Offset>
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
	Iterator m_it;
	Count m_count;
};

template<typename Iterator, typename Count>
struct random_access_counted_iterator {
	using difference_type = decltype(bounded::declval<Count>() - bounded::declval<Count>());

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
		return ::bounded::assume_in_range<difference_type>(std::move(lhs).m_it - std::move(rhs).m_it);
	}

	friend auto & operator++(random_access_counted_iterator & it) requires(numeric_traits::max_value<difference_type> == 0_bi) {
		std::unreachable();
		return it;
	}

private:
	Iterator m_it;
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
			using namespace std::string_view_literals;
			throw std::runtime_error(containers::concatenate<std::string>(
				"Did not take all elements of range. size(source) == "sv,
				to_any_string<std::string>(source_size),
				", given count == "sv,
				to_any_string<std::string>(count)
			));
		}
		return containers::range_view(
			containers::begin(OPERATORS_FORWARD(source)),
			containers::end(OPERATORS_FORWARD(source))
		);
	} else if constexpr (sized_range<Range> and forward_random_access_iterator<iterator>) {
		auto const true_count = bounded::min(containers::size(source), count);
		auto first = random_access_counted_iterator<iterator, count_type>(containers::begin(OPERATORS_FORWARD(source)));
		return containers::range_view(first, first + true_count);
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

using sized_range_t = containers::array<int, 8_bi>;
constexpr auto sized_range = sized_range_t{0, 1, 2, 3, 4, 5, 6, 7};

inline namespace adl {

BOUNDED_COMMON_ARITHMETIC()

struct unsized_range_t {
	struct const_iterator {
		using underlying_iterator = containers::iterator_t<sized_range_t const>;
		using difference_type = containers::iter_difference_t<underlying_iterator>;
		constexpr explicit const_iterator(underlying_iterator const it):
			m_it(it)
		{
		}
		constexpr auto & operator*() const {
			return *m_it;
		}
		OPERATORS_ARROW_DEFINITIONS

		friend constexpr auto operator+(const_iterator const lhs, bounded::constant_t<1> const rhs) {
			return const_iterator(lhs.m_it + rhs);
		}

		friend auto operator==(const_iterator, const_iterator) -> bool = default;

	private:
		underlying_iterator m_it;
	};
	constexpr auto begin() const {
		return const_iterator(containers::begin(sized_range));
	}
	constexpr auto end() const {
		return const_iterator(containers::end(sized_range));
	}
};
constexpr auto unsized_range = unsized_range_t();

} // inline namespace adl

constexpr auto test_take(auto const & source) {
	BOUNDED_ASSERT(containers::equal(containers::take(source, 8_bi), sized_range));
	BOUNDED_ASSERT(containers::equal(containers::take(source, 4_bi), containers::array{0, 1, 2, 3}));
	BOUNDED_ASSERT(containers::equal(containers::take(source, 1_bi), containers::array{0}));
	BOUNDED_ASSERT(containers::equal(containers::take(source, 0_bi), containers::array<int, 0_bi>()));
	BOUNDED_ASSERT(containers::equal(containers::take(source, 12_bi), sized_range));
	return true;
}
static_assert(test_take(sized_range));
static_assert(containers::take(sized_range, 4_bi)[3_bi] == 3);
static_assert(test_take(unsized_range));

template<typename Range, auto index>
concept takeable = requires(Range range) { containers::take(range, index); };

static_assert(!takeable<sized_range_t, -1_bi>);
static_assert(!takeable<unsized_range_t, -1_bi>);


constexpr auto test_check_size_not_greater_than(auto const & source) {
	BOUNDED_ASSERT(containers::equal(containers::check_size_not_greater_than(source, 8_bi), sized_range));
	BOUNDED_ASSERT(containers::equal(containers::check_size_not_greater_than(source, 12_bi), sized_range));
	return true;
}
static_assert(test_check_size_not_greater_than(sized_range));
static_assert(containers::check_size_not_greater_than(sized_range, 8_bi)[3_bi] == 3);
static_assert(test_check_size_not_greater_than(unsized_range));

template<typename Range, auto index>
concept check_sizable = requires(Range range) { containers::check_size_not_greater_than(range, index); };

static_assert(!check_sizable<sized_range_t, -1_bi>);
static_assert(!check_sizable<unsized_range_t, -1_bi>);
static_assert(!check_sizable<sized_range_t, 1_bi>);
static_assert(check_sizable<unsized_range_t, 1_bi>);
