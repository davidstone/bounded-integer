// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

module;

#include <bounded/assert.hpp>

#include <operators/arrow.hpp>
#include <operators/bracket.hpp>
#include <operators/forward.hpp>

export module containers.algorithms.concatenate_view;

import containers.algorithms.compare;
import containers.array;
import containers.begin_end;
import containers.common_iterator_functions;
import containers.forward_random_access_range;
import containers.front_back;
import containers.is_empty;
import containers.is_iterator;
import containers.is_iterator_sentinel;
import containers.iter_difference_t;
import containers.iterator_t;
import containers.range;
import containers.range_reference_t;
import containers.range_value_t;
import containers.range_view;
import containers.size;

import bounded;
import numeric_traits;
import operators;
import tv;
import std_module;

using namespace bounded::literal;

namespace containers {

template<typename LHS, typename RHS, std::size_t... indexes>
constexpr auto have_same_ends(LHS const & lhs, RHS const & rhs, std::index_sequence<indexes...>) {
	return (... and (containers::end(lhs[bounded::constant<indexes>]) == containers::end(rhs[bounded::constant<indexes>])));
}

template<typename LHS, typename RHS>
constexpr auto assert_same_ends(LHS const & lhs, RHS const & rhs) {
	static_assert(tv::tuple_size<LHS> == tv::tuple_size<RHS>);
	BOUNDED_ASSERT(have_same_ends(lhs, rhs, bounded::make_index_sequence(tv::tuple_size<LHS>)));
}

template<typename Category, typename RangeView>
concept one_is_category = std::same_as<typename iterator_t<RangeView>::iterator_category, Category>;

template<typename Category, typename... RangeViews>
inline constexpr bool any_is_category = (
	... or
	one_is_category<Category, RangeViews>
);

template<typename... RangeViews>
inline constexpr bool any_is_input_iterator = any_is_category<std::input_iterator_tag, RangeViews...>;

template<typename... RangeViews>
inline constexpr bool any_is_output_iterator = any_is_category<std::output_iterator_tag, RangeViews...>;


// This is an interesting iterator type. If the iterators of the underlying
// ranges support it, concatenate_view_iterator can efficiently support
// subtracting two iterators in constant time and random forward access. They
// cannot, however, support moving an iterator backward. Doing so would require
// either storing more state (the original begin and end and the current
// position in that range, which is one extra iterator) or require storing an
// iterator + reference (which would mean that more operations would require
// dereferencing). This puts us in the strange place where
// `it + offset - it == offset` holds, but `it + offset - offset == it` cannot
// be computed.
export template<typename... RangeViews>
struct concatenate_view_iterator {
	static_assert((... and is_range_view<RangeViews>));
	static_assert(
		!any_is_input_iterator<RangeViews...> or !any_is_output_iterator<RangeViews...>,
		"Cannot combine input and output ranges in concatenate_view"
	);

	using difference_type = decltype((0_bi + ... + bounded::declval<iter_difference_t<iterator_t<RangeViews>>>()));
	
	using iterator_category =
		std::conditional_t<any_is_output_iterator<RangeViews...>, std::output_iterator_tag,
		std::conditional_t<any_is_input_iterator<RangeViews...>, std::input_iterator_tag,
		std::forward_iterator_tag
	>>;
	
	using reference = bounded::common_type_and_value_category<
		range_reference_t<RangeViews>...
	>;

	constexpr explicit concatenate_view_iterator(RangeViews... range_views):
		m_range_views(range_views...)
	{
	}

	constexpr explicit concatenate_view_iterator(tv::tuple<RangeViews...> range_views):
		m_range_views(std::move(range_views))
	{
	}


	constexpr decltype(auto) operator*() const {
		// TODO: Implement with an expansion statement `for...`
		return operator_star(0_bi);
	}
	OPERATORS_ARROW_DEFINITIONS

	template<typename Offset> requires(
		std::same_as<Offset, bounded::constant_t<1>> or
		(!bounded::bounded_integer<difference_type> and numeric_traits::max_value<Offset> >= 0_bi and (... and forward_random_access_range<RangeViews>)) or
		(
			bounded::convertible_to<Offset, difference_type> and
			numeric_traits::min_value<Offset> >= 0_bi and
			(... and forward_random_access_range<RangeViews>)
		)
	)
	friend constexpr auto operator+(concatenate_view_iterator const lhs, Offset const offset) {
		return [=]<std::size_t... indexes>(std::index_sequence<indexes...>) {
			BOUNDED_ASSERT(offset >= 0_bi);
			auto remaining_offset = bounded::integer<0, bounded::builtin_max_value<Offset>>(::bounded::assume_in_range(offset, 0_bi, bounded::integer(numeric_traits::max_value<Offset>)));
			auto specific_range = [&](auto const index) {
				auto const range = lhs.m_range_views[index];
				auto const added_size = bounded::min(containers::size(range), remaining_offset);
				remaining_offset -= added_size;
				return range_view(containers::begin(range) + added_size, containers::end(range));
			};
			// Use {} to enforce initialization order
			return tv::apply(
				tv::tuple{specific_range(bounded::constant<indexes>)...},
				bounded::construct<concatenate_view_iterator>
			);
		}(std::make_index_sequence<sizeof...(RangeViews)>());
	}

	template<typename... RHSRanges>
	friend constexpr auto operator-(
		concatenate_view_iterator const lhs,
		concatenate_view_iterator<RHSRanges...> const rhs
	) {
		::containers::assert_same_ends(lhs.m_range_views, rhs.m_range_views);

		auto transform = [](auto const lhs_range, auto const rhs_range) {
			return containers::begin(lhs_range) - containers::begin(rhs_range);
		};
		return tv::apply(
			tv::transform(transform, lhs.m_range_views, rhs.m_range_views),
			[](auto... integers) { return (... + integers); }
		);
	}


	friend constexpr auto operator-(
		std::default_sentinel_t,
		concatenate_view_iterator const rhs
	) {
		auto transform = [](auto const range) { return containers::size(range); };
		return tv::apply(
			tv::transform(transform, rhs.m_range_views),
			[](auto... integers) { return (... + integers); }
		);
	}

	friend constexpr auto operator-(concatenate_view_iterator const lhs, std::default_sentinel_t const rhs) {
		return -(rhs - lhs);
	}

	template<typename... RHSRanges>
	friend constexpr auto operator<=>(
		concatenate_view_iterator const lhs,
		concatenate_view_iterator<RHSRanges...> const rhs
	) {
		::containers::assert_same_ends(lhs.m_range_views, rhs.m_range_views);
		return lhs.begin_iterators() <=> rhs.begin_iterators();
	}

	friend constexpr auto operator<=>(concatenate_view_iterator const lhs, std::default_sentinel_t const rhs) {
		return lhs == rhs ? std::strong_ordering::equal : std::strong_ordering::less;
	}


	template<typename... RHSRanges>
	friend constexpr auto operator==(concatenate_view_iterator const lhs, concatenate_view_iterator<RHSRanges...> const rhs) -> bool {
		::containers::assert_same_ends(lhs.m_range_views, rhs.m_range_views);
		return lhs.begin_iterators() == rhs.begin_iterators();
	}

	friend constexpr auto operator==(concatenate_view_iterator const lhs, std::default_sentinel_t) -> bool {
		auto get_end_iterators = [](auto const range) { return containers::end(range); };
		return lhs.begin_iterators() == tv::transform(get_end_iterators, lhs.m_range_views);
	}

private:
	static constexpr auto max_index = bounded::constant<sizeof...(RangeViews)> - 1_bi;

	constexpr auto operator_star(auto const index) const -> reference {
		auto const range_view = m_range_views[index];
		if constexpr (index == max_index) {
			return containers::front(range_view);
		} else if (!containers::is_empty(range_view)) {
			return containers::front(range_view);
		} else {
			return operator_star(index + 1_bi);
		}
	}

	constexpr auto begin_iterators() const {
		return tv::transform([](auto const range) { return containers::begin(range); }, m_range_views);
	}

	tv::tuple<RangeViews...> m_range_views;
};




export template<typename... Ranges>
struct concatenate_view {
	constexpr concatenate_view(Ranges && ... ranges):
		m_ranges(OPERATORS_FORWARD(ranges)...)
	{
	}
	
	constexpr auto begin() const {
		return begin_impl(m_ranges);
	}
	constexpr auto begin() {
		return begin_impl(m_ranges);
	}
	static constexpr auto end() {
		return std::default_sentinel;
	}

	OPERATORS_BRACKET_SEQUENCE_RANGE_DEFINITIONS
private:
	static constexpr auto begin_impl(auto && ranges) {
		return concatenate_view_iterator(tv::transform(
			[](auto && range){ return range_view(containers::begin(range), containers::end(range)); },
			ranges
		));
	}
	tv::tuple<Ranges...> m_ranges;
};

template<typename... Ranges>
concatenate_view(Ranges && ...) -> concatenate_view<Ranges...>;


}	// namespace containers

template<typename LHS, typename RHS>
constexpr bool equal_values_and_types(LHS const & lhs, RHS const & rhs) {
	static_assert(std::same_as<containers::range_value_t<LHS>, containers::range_value_t<RHS>>);
	return containers::equal(lhs, rhs);
}

constexpr auto array1 = containers::array{1, 2, 3};
constexpr auto array2 = containers::array{2, 3, 5, 7};
constexpr auto array3 = containers::array{1, 1, 2, 3, 5};

constexpr auto two = containers::concatenate_view(array1, array2);

using iterator = decltype(begin(two));
using sentinel = decltype(end(two));

static_assert(containers::iterator<iterator>);
static_assert(containers::sentinel_for<sentinel, iterator>);
static_assert(containers::range<decltype(two)>);

static_assert(containers::size(two) == containers::size(array1) + containers::size(array2));
static_assert(equal_values_and_types(
	two,
	containers::array{1, 2, 3, 2, 3, 5, 7}
));
static_assert(begin(two) + containers::size(two) == end(two));
static_assert(begin(two) + containers::size(two) - begin(two) == containers::size(two));

constexpr auto three = containers::concatenate_view(array3, array1, array2);
static_assert(containers::size(three) == containers::size(array3) + containers::size(array1) + containers::size(array2));
static_assert(equal_values_and_types(
	three,
	containers::array{1, 1, 2, 3, 5, 1, 2, 3, 2, 3, 5, 7}
));
static_assert(begin(three) + containers::size(three) == end(three));
static_assert(begin(three) + containers::size(three) - begin(three) == containers::size(three));

static_assert(*(begin(three) + 7_bi) == 3);

constexpr auto from_temp = containers::concatenate_view(containers::array{1}, containers::array{2});
static_assert(equal_values_and_types(from_temp, containers::array{1, 2}));

static_assert(containers::forward_random_access_range<containers::concatenate_view<std::string_view>>);
