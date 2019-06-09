// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/algorithms/move_iterator.hpp>
#include <containers/front_back.hpp>
#include <containers/is_range.hpp>
#include <containers/operator_arrow.hpp>
#include <containers/range_view.hpp>

#include <bounded/detail/make_index_sequence.hpp>
#include <bounded/detail/tuple.hpp>
#include <bounded/integer.hpp>

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

} // namespace detail

template<typename... RangeViews>
struct concatenate_view_iterator : detail::operator_arrow<concatenate_view_iterator<RangeViews...>> {
private:
	template<typename RangeView>
	using traits = std::iterator_traits<decltype(begin(std::declval<RangeView>()))>;

	template<typename category>
	static constexpr auto any_is_category =
		(... or std::is_same_v<typename traits<RangeViews>::iterator_category, category>);

	static_assert(!any_is_category<std::output_iterator_tag> or !any_is_category<std::input_iterator_tag>);

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
		} else if constexpr (std::is_convertible_v<iterator_category, std::random_access_iterator_tag>) {
			auto const range = range_views[index];
			auto const added_size = bounded::min(offset, size(range));
			auto specific_range = [=](auto const current_index) {
				auto const current_range = range_views[current_index];
				if constexpr (index == current_index) {
					return range_view(begin(current_range) + added_size, end(current_range));
				} else {
					return current_range;
				}
			};
			auto const temp = bounded::tuple(specific_range(bounded::constant<indexes>)...);
			return operator_plus(temp, index + bounded::constant<1>, offset - added_size, indexes_);
		} else {
			auto const range = range_views[index];
			auto specific_range = [=](auto const current_index) {
				auto const current_range = range_views[current_index];
				if constexpr (index == current_index) {
					return range_view(std::next(begin(current_range)), end(current_range));
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
	};


public:
	using value_type = bounded::detail::common_type_and_value_category_t<
		typename traits<RangeViews>::value_type...
	>;
	using difference_type = decltype((... + std::declval<typename traits<RangeViews>::difference_type>()));
	
	using iterator_category =
		std::conditional_t<any_is_category<std::output_iterator_tag>, std::output_iterator_tag,
		std::conditional_t<any_is_category<std::input_iterator_tag>, std::input_iterator_tag,
		std::conditional_t<any_is_category<std::forward_iterator_tag>, std::forward_iterator_tag,
		std::conditional_t<any_is_category<std::bidirectional_iterator_tag>, std::bidirectional_iterator_tag,
		std::random_access_iterator_tag
	>>>>;
	
	using pointer = std::remove_reference_t<value_type> *;

	using reference = bounded::detail::common_type_and_value_category_t<
		typename traits<RangeViews>::reference...
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

	template<typename Offset, BOUNDED_REQUIRES(
		std::is_convertible_v<Offset, difference_type> and
		(
			!bounded::is_bounded_integer<difference_type> or
			std::is_same_v<iterator_category, std::random_access_iterator_tag> or
			std::numeric_limits<Offset>::max() == bounded::constant<1>
		)
	)>
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
		assert_same_ends(lhs.m_range_views, rhs.m_range_views);

		auto transform = [](auto const lhs_range, auto const rhs_range) {
			return begin(lhs_range) - begin(rhs_range);
		};
		return bounded::apply(
			bounded::transform(transform, lhs.m_range_views, rhs.m_range_views),
			std::plus{}
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
	friend constexpr auto compare(
		concatenate_view_iterator const lhs,
		concatenate_view_iterator<RHSRanges...> const rhs
	) {
		assert_same_ends(lhs.m_range_views, rhs.m_range_views);
		return compare(lhs.begin_iterators(), rhs.begin_iterators());
	}

	friend constexpr auto compare(concatenate_view_iterator const lhs, concatenate_view_sentinel const rhs) BOUNDED_NOEXCEPT(
		lhs == rhs ? bounded::strong_ordering::equal : bounded::strong_ordering::less
	)


	template<typename... RHSRanges>
	friend constexpr auto operator==(concatenate_view_iterator const lhs, concatenate_view_iterator<RHSRanges...> const rhs) {
		assert_same_ends(lhs.m_range_views, rhs.m_range_views);
		return lhs.begin_iterators() == rhs.begin_iterators();
	}

	friend constexpr auto operator==(concatenate_view_iterator const lhs, concatenate_view_sentinel) {
		auto get_end_iterators = [](auto const range) { return end(range); };
		return lhs.begin_iterators() == bounded::transform(get_end_iterators, lhs.m_range_views);
	}


	template<typename Index>
	constexpr auto operator[](Index const index) const noexcept(noexcept(*(std::declval<concatenate_view_iterator>() + index))) -> decltype(*(*this + index)) {
		return *(*this + index);
	}
};


template<typename... Ranges>
constexpr auto operator-(concatenate_view_iterator<Ranges...> const lhs, concatenate_view_sentinel const rhs) BOUNDED_NOEXCEPT_DECLTYPE(
	-(rhs - lhs)
)


template<typename... Ranges>
constexpr auto compare(concatenate_view_sentinel const lhs, concatenate_view_iterator<Ranges...> const rhs) BOUNDED_NOEXCEPT_DECLTYPE(
	compare(0, compare(rhs, lhs))
)


template<typename... Ranges>
constexpr auto operator==(concatenate_view_sentinel const lhs, concatenate_view_iterator<Ranges...> const rhs) BOUNDED_NOEXCEPT_DECLTYPE(
	rhs == lhs
)

template<typename... Ranges>
struct concatenate_view {
	using const_iterator = concatenate_view_iterator<decltype(range_view(std::declval<Ranges const &>()))...>;
	using iterator = concatenate_view_iterator<decltype(range_view(std::declval<Ranges &>()))...>;
	using value_type = typename iterator::value_type;
	using size_type = bounded::integer<
		0,
		bounded::detail::normalize<std::numeric_limits<typename iterator::difference_type>::max().value()>
	>;
	
	constexpr concatenate_view(Ranges && ... ranges) noexcept((... and std::is_nothrow_constructible_v<Ranges, Ranges &&>)):
		m_ranges(BOUNDED_FORWARD(ranges)...)
	{
	}
	
	friend constexpr auto begin(concatenate_view & view) {
		return iterator(bounded::transform([](auto && range){ return range_view(range); }, view.m_ranges));
	}
	friend constexpr auto begin(concatenate_view const & view) {
		return const_iterator(bounded::transform([](auto && range){ return range_view(range); }, view.m_ranges));
	}
	friend constexpr auto end(concatenate_view & range) {
		return concatenate_view_sentinel();
	}
	friend constexpr auto end(concatenate_view const & range) {
		return concatenate_view_sentinel();
	}

private:
	bounded::tuple<Ranges...> m_ranges;
};

template<typename... Ranges>
concatenate_view(Ranges && ...) -> concatenate_view<Ranges...>;


}	// namespace containers
