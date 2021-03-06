// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/algorithms/move_iterator.hpp>
#include <containers/begin_end.hpp>
#include <containers/common_iterator_functions.hpp>
#include <containers/front_back.hpp>
#include <containers/is_empty.hpp>
#include <containers/is_range.hpp>
#include <containers/iter_difference_t.hpp>
#include <containers/iter_value_t.hpp>
#include <containers/iterator_t.hpp>
#include <containers/range_value_t.hpp>
#include <containers/range_view.hpp>
#include <containers/size.hpp>

#include <bounded/detail/construct_destroy.hpp>
#include <bounded/detail/make_index_sequence.hpp>
#include <bounded/detail/tuple.hpp>
#include <bounded/integer.hpp>
#include <bounded/std_iterator.hpp>

#include <numeric_traits/min_max_value.hpp>

#include <operators/operators.hpp>

namespace containers {

struct concatenate_view_sentinel {
};

namespace detail {

template<typename LHS, typename RHS, std::size_t... indexes>
constexpr auto have_same_ends(LHS const & lhs, RHS const & rhs, std::index_sequence<indexes...>) {
	return (... and (containers::end(lhs[bounded::constant<indexes>]) == containers::end(rhs[bounded::constant<indexes>])));
}

template<typename LHS, typename RHS>
constexpr auto assert_same_ends(LHS const & lhs, RHS const & rhs) {
	static_assert(bounded::tuple_size<LHS> == bounded::tuple_size<RHS>);
	BOUNDED_ASSERT(have_same_ends(lhs, rhs, bounded::make_index_sequence(bounded::tuple_size<LHS>)));
}

template<typename RangeView>
using view_iterator_traits = std::iterator_traits<iterator_t<RangeView>>;

template<typename category, typename... RangeViews>
inline constexpr bool any_is_category = (
	... or
	std::is_same_v<typename view_iterator_traits<RangeViews>::iterator_category, category>
);

template<typename... RangeViews>
inline constexpr bool any_is_input_iterator = any_is_category<std::input_iterator_tag, RangeViews...>;

template<typename... RangeViews>
inline constexpr bool any_is_output_iterator = any_is_category<std::output_iterator_tag, RangeViews...>;


} // namespace detail

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
template<typename... RangeViews>
struct concatenate_view_iterator {
	static_assert((... and is_range_view<RangeViews>));
	static_assert(
		!detail::any_is_input_iterator<RangeViews...> or !detail::any_is_output_iterator<RangeViews...>,
		"Cannot combine input and output ranges in concatenate_view"
	);

	using value_type = bounded::detail::common_type_and_value_category_t<
		range_value_t<RangeViews>...
	>;
	using difference_type = decltype((0_bi + ... + std::declval<iter_difference_t<iterator_t<RangeViews>>>()));
	
	using iterator_category =
		std::conditional_t<detail::any_is_output_iterator<RangeViews...>, std::output_iterator_tag,
		std::conditional_t<detail::any_is_input_iterator<RangeViews...>, std::input_iterator_tag,
		std::forward_iterator_tag
	>>;
	
	using pointer = std::remove_reference_t<value_type> *;

	using reference = bounded::detail::common_type_and_value_category_t<
		typename detail::view_iterator_traits<RangeViews>::reference...
	>;

	constexpr explicit concatenate_view_iterator(RangeViews... range_views):
		m_range_views(range_views...)
	{
	}

	constexpr explicit concatenate_view_iterator(bounded::tuple<RangeViews...> range_views):
		m_range_views(std::move(range_views))
	{
	}


	constexpr decltype(auto) operator*() const {
		// TODO: Implement with an expansion statement `for...`
		return operator_star(bounded::constant<0>);
	}
	OPERATORS_ARROW_DEFINITIONS

	template<typename Offset> requires(
		std::is_same_v<Offset, bounded::constant_t<1>> or
		(!bounded::bounded_integer<difference_type> and numeric_traits::max_value<Offset> >= bounded::constant<0> and (... and forward_random_access_range<RangeViews>)) or
		(
			bounded::convertible_to<Offset, difference_type> and
			numeric_traits::min_value<Offset> >= bounded::constant<0> and
			(... and forward_random_access_range<RangeViews>)
		)
	)
	friend constexpr auto operator+(concatenate_view_iterator const lhs, Offset const offset) {
		return [=]<std::size_t... indexes>(std::index_sequence<indexes...>) {
			BOUNDED_ASSERT(offset >= bounded::constant<0>);
			auto remaining_offset = bounded::integer<0, bounded::builtin_max_value<Offset>>(offset);
			auto specific_range = [&](auto const index) {
				auto const range = lhs.m_range_views[index];
				using size_type = range_size_t<decltype(range)>;
				auto const added_size = size_type(bounded::min(containers::size(range), remaining_offset));
				remaining_offset -= added_size;
				return range_view(containers::begin(range) + size_type(added_size), containers::end(range));
			};
			// Use {} to enforce initialization order
			return bounded::apply(
				bounded::tuple{specific_range(bounded::constant<indexes>)...},
				bounded::construct_return<concatenate_view_iterator>
			);
		}(std::make_index_sequence<sizeof...(RangeViews)>());
	}

	template<typename... RHSRanges>
	friend constexpr auto operator-(
		concatenate_view_iterator const lhs,
		concatenate_view_iterator<RHSRanges...> const rhs
	) {
		::containers::detail::assert_same_ends(lhs.m_range_views, rhs.m_range_views);

		auto transform = [](auto const lhs_range, auto const rhs_range) {
			return containers::begin(lhs_range) - containers::begin(rhs_range);
		};
		return bounded::apply(
			bounded::transform(transform, lhs.m_range_views, rhs.m_range_views),
			[](auto... integers) { return (... + integers); }
		);
	}


	friend constexpr auto operator-(
		concatenate_view_sentinel,
		concatenate_view_iterator const rhs
	) {
		auto transform = [](auto const range) { return containers::size(range); };
		return bounded::apply(
			bounded::transform(transform, rhs.m_range_views),
			[](auto... integers) { return (... + integers); }
		);
	}


	template<typename... RHSRanges>
	friend constexpr auto operator<=>(
		concatenate_view_iterator const lhs,
		concatenate_view_iterator<RHSRanges...> const rhs
	) {
		::containers::detail::assert_same_ends(lhs.m_range_views, rhs.m_range_views);
		return lhs.begin_iterators() <=> rhs.begin_iterators();
	}

	friend constexpr auto operator<=>(concatenate_view_iterator const lhs, concatenate_view_sentinel const rhs) {
		return lhs == rhs ? std::strong_ordering::equal : std::strong_ordering::less;
	}


	template<typename... RHSRanges>
	friend constexpr auto operator==(concatenate_view_iterator const lhs, concatenate_view_iterator<RHSRanges...> const rhs) -> bool {
		::containers::detail::assert_same_ends(lhs.m_range_views, rhs.m_range_views);
		return lhs.begin_iterators() == rhs.begin_iterators();
	}

	friend constexpr auto operator==(concatenate_view_iterator const lhs, concatenate_view_sentinel) -> bool {
		auto get_end_iterators = [](auto const range) { return containers::end(range); };
		return lhs.begin_iterators() == bounded::transform(get_end_iterators, lhs.m_range_views);
	}

private:
	static constexpr auto max_index = bounded::constant<sizeof...(RangeViews)> - bounded::constant<1>;

	constexpr auto operator_star(auto const index) const -> reference {
		auto const range_view = m_range_views[index];
		if constexpr (index == max_index) {
			return containers::front(range_view);
		} else if (!containers::is_empty(range_view)) {
			return containers::front(range_view);
		} else {
			return operator_star(index + bounded::constant<1>);
		}
	}

	constexpr auto begin_iterators() const {
		return bounded::transform([](auto const range) { return containers::begin(range); }, m_range_views);
	}

	bounded::tuple<RangeViews...> m_range_views;
};


template<typename... Ranges>
constexpr auto operator-(concatenate_view_iterator<Ranges...> const lhs, concatenate_view_sentinel const rhs) OPERATORS_RETURNS(
	-(rhs - lhs)
)


template<typename... Ranges>
struct concatenate_view {
	using const_iterator = concatenate_view_iterator<decltype(range_view(std::declval<Ranges const &>()))...>;
	using iterator = concatenate_view_iterator<decltype(range_view(std::declval<Ranges &>()))...>;
	using value_type = iter_value_t<iterator>;
	
	constexpr concatenate_view(Ranges && ... ranges):
		m_ranges(OPERATORS_FORWARD(ranges)...)
	{
	}
	
	constexpr auto begin() const & {
		return const_iterator(bounded::transform([](auto && range){ return range_view(range); }, m_ranges));
	}
	constexpr auto begin() & {
		return iterator(bounded::transform([](auto && range){ return range_view(range); }, m_ranges));
	}
	static constexpr auto end() {
		return concatenate_view_sentinel();
	}

	OPERATORS_BRACKET_SEQUENCE_RANGE_DEFINITIONS
private:
	bounded::tuple<Ranges...> m_ranges;
};

template<typename... Ranges>
concatenate_view(Ranges && ...) -> concatenate_view<Ranges...>;


}	// namespace containers
