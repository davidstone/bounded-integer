// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/algorithms/move_iterator.hpp>
#include <containers/front_back.hpp>
#include <containers/is_range.hpp>
#include <containers/range_view.hpp>

#include <bounded/detail/construct_destroy.hpp>
#include <bounded/detail/make_index_sequence.hpp>
#include <bounded/detail/tuple.hpp>
#include <bounded/integer.hpp>

#include <operators/operators.hpp>

namespace containers {

struct concatenate_view_sentinel {
};

namespace detail {

template<typename LHS, typename RHS, std::size_t... indexes>
constexpr auto have_same_ends(LHS const & lhs, RHS const & rhs, std::index_sequence<indexes...>) {
	return (... and (end(lhs[bounded::constant<indexes>]) == end(rhs[bounded::constant<indexes>])));
}

template<typename LHS, typename RHS>
constexpr auto assert_same_ends(LHS const & lhs, RHS const & rhs) {
	static_assert(bounded::tuple_size<LHS> == bounded::tuple_size<RHS>);
	BOUNDED_ASSERT(have_same_ends(lhs, rhs, bounded::make_index_sequence(bounded::tuple_size<LHS>)));
}

template<typename RangeView>
using view_iterator = decltype(begin(std::declval<RangeView>()));

template<typename RangeView>
using view_iterator_traits = std::iterator_traits<view_iterator<RangeView>>;

template<typename category, typename... RangeViews>
inline constexpr bool any_is_category = (
	... or
	std::is_same_v<typename view_iterator_traits<RangeViews>::iterator_category, category>
);

template<typename... RangeViews>
inline constexpr bool any_is_input_iterator = any_is_category<std::input_iterator_tag, RangeViews...>;

template<typename... RangeViews>
inline constexpr bool any_is_output_iterator = any_is_category<std::output_iterator_tag, RangeViews...>;


template<typename Iterator>
concept forward_random_access_iterator = iterator<Iterator> and iterator<decltype(std::declval<Iterator>() + std::declval<index_type<std::decay_t<Iterator>>>())>;

template<typename Range>
concept forward_random_access_range = range<Range> and forward_random_access_iterator<decltype(begin(std::declval<Range>()))>;

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
private:
	static_assert((... and is_range_view<RangeViews>));

	static_assert(
		!detail::any_is_input_iterator<RangeViews...> or !detail::any_is_output_iterator<RangeViews...>,
		"Cannot combine input and output ranges in concatenate_view"
	);

	bounded::tuple<RangeViews...> m_range_views;

	static constexpr auto index_sequence = std::make_index_sequence<sizeof...(RangeViews)>{};
	static constexpr auto max_index = bounded::constant<sizeof...(RangeViews)> - bounded::constant<1>;

	template<typename Index>
	constexpr decltype(auto) operator_star(Index const index) const {
		auto const range_view = m_range_views[index];
		if constexpr (index == max_index) {
			return front(range_view);
		} else if (!empty(range_view)) {
			return front(range_view);
		} else {
			return operator_star(index + bounded::constant<1>);
		}
	}

	template<typename Index, typename Offset, std::size_t... indexes>
	static constexpr auto operator_plus(bounded::tuple<RangeViews...> const range_views, Index const index, Offset const offset, std::index_sequence<indexes...> indexes_) {
		if constexpr (index == sizeof...(RangeViews)) {
			return bounded::apply(range_views, bounded::construct_return<concatenate_view_iterator>);
		} else if constexpr ((... and detail::forward_random_access_range<RangeViews>)) {
			auto const range = range_views[index];
			auto const added_size = bounded::max(-size(range), bounded::min(size(range), offset));
			auto specific_range = [=](auto const current_index) {
				auto const current_range = range_views[current_index];
				if constexpr (index == current_index) {
					return range_view(begin(current_range) + added_size, end(current_range));
				} else {
					return current_range;
				}
			};
			auto const temp = bounded::tuple(specific_range(bounded::constant<indexes>)...);
			return operator_plus(
				temp,
				index + bounded::constant<1>,
				offset - added_size,
				indexes_
			);
		} else {
			auto const range = range_views[index];
			auto specific_range = [=](auto const current_index) {
				auto const current_range = range_views[current_index];
				if constexpr (index == current_index) {
					return range_view(::containers::next(begin(current_range)), end(current_range));
				} else {
					return current_range;
				}
			};
			auto const temp = bounded::tuple(specific_range(bounded::constant<indexes>)...);
			return empty(range) ?
				operator_plus(temp, index + bounded::constant<1>, offset, indexes_) :
				operator_plus(temp, index + bounded::constant<1>, bounded::constant<0>, indexes_);
		}
	}

	constexpr auto begin_iterators() const {
		return bounded::transform([](auto const range) { return begin(range); }, m_range_views);
	}


public:
	using value_type = bounded::detail::common_type_and_value_category_t<
		typename detail::view_iterator_traits<RangeViews>::value_type...
	>;
	using difference_type = decltype((... + std::declval<typename detail::view_iterator_traits<RangeViews>::difference_type>()));
	
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
	OPERATORS_BRACKET_ITERATOR_DEFINITIONS

	template<typename Offset> requires(
		std::is_convertible_v<Offset, difference_type> and
		(
			!bounded::bounded_integer<difference_type> or
			(
				bounded::min_value<Offset> == bounded::constant<1> and
				bounded::max_value<Offset> == bounded::constant<1>
			) or
			(
				bounded::min_value<Offset> >= bounded::constant<0> and
				(... and detail::forward_random_access_range<RangeViews>)
			)
		)
	)
	friend constexpr auto operator+(concatenate_view_iterator const lhs, Offset const offset) {
		return operator_plus(
			lhs.m_range_views,
			bounded::constant<0>,
			offset,
			std::make_index_sequence<sizeof...(RangeViews)>{}
		);
	}

	template<typename... RHSRanges>
	friend constexpr auto operator-(
		concatenate_view_iterator const lhs,
		concatenate_view_iterator<RHSRanges...> const rhs
	) {
		::containers::detail::assert_same_ends(lhs.m_range_views, rhs.m_range_views);

		auto transform = [](auto const lhs_range, auto const rhs_range) {
			return begin(lhs_range) - begin(rhs_range);
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
		auto transform = [](auto const range) { return size(range); };
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
		auto get_end_iterators = [](auto const range) { return end(range); };
		return lhs.begin_iterators() == bounded::transform(get_end_iterators, lhs.m_range_views);
	}
};


template<typename... Ranges>
constexpr auto operator-(concatenate_view_iterator<Ranges...> const lhs, concatenate_view_sentinel const rhs) OPERATORS_RETURNS(
	-(rhs - lhs)
)


template<typename... Ranges>
struct concatenate_view {
	using const_iterator = concatenate_view_iterator<decltype(range_view(std::declval<Ranges const &>()))...>;
	using iterator = concatenate_view_iterator<decltype(range_view(std::declval<Ranges &>()))...>;
	using value_type = typename iterator::value_type;
	using size_type = bounded::integer<
		0,
		bounded::detail::normalize<bounded::max_value<typename iterator::difference_type>.value()>
	>;
	
	constexpr concatenate_view(Ranges && ... ranges):
		m_ranges(OPERATORS_FORWARD(ranges)...)
	{
	}
	
	friend constexpr auto begin(concatenate_view & view) {
		return iterator(bounded::transform([](auto && range){ return range_view(range); }, view.m_ranges));
	}
	friend constexpr auto begin(concatenate_view const & view) {
		return const_iterator(bounded::transform([](auto && range){ return range_view(range); }, view.m_ranges));
	}
	friend constexpr auto end(concatenate_view) {
		return concatenate_view_sentinel();
	}

	OPERATORS_BRACKET_SEQUENCE_RANGE_DEFINITIONS
private:
	bounded::tuple<Ranges...> m_ranges;
};

template<typename... Ranges>
concatenate_view(Ranges && ...) -> concatenate_view<Ranges...>;


}	// namespace containers
