// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

module;

#include <bounded/assert.hpp>

#include <operators/forward.hpp>

export module containers.algorithms.set;

import containers.algorithms.advance;
import containers.algorithms.compare;
import containers.algorithms.is_sorted;
import containers.array;
import containers.begin_end;
import containers.common_iterator_functions;
import containers.is_range;
import containers.iter_difference_t;
import containers.iter_value_t;

import bounded;
import numeric_traits;
import std_module;

namespace containers {

constexpr auto less_to_compare = [](auto const & cmp) {
	return [&](auto const & lhs, auto const & rhs) {
		using result_t = decltype(cmp(lhs, rhs));
		if constexpr (bounded::convertible_to<result_t, std::strong_ordering>) {
			return cmp(lhs, rhs);
		} else {
			static_assert(bounded::constructible_from<bool, result_t>);
			return
				cmp(lhs, rhs) ? std::strong_ordering::less :
				cmp(rhs, lhs) ? std::strong_ordering::greater :
				std::strong_ordering::equal;
		}
	};
};

template<typename Members, typename ForwardIterator1, typename ForwardIterator2>
struct set_intersection_pair_iterator;

template<typename Range1, typename Range2, typename Compare>
struct set_intersection_members {
	Range1 range1;
	Range2 range2;
	Compare compare;
};

constexpr auto find_first_matching(auto & members, auto it1, auto it2) {
	auto const last1 = ::containers::end(members.range1);
	auto const last2 = ::containers::end(members.range2);
	auto const comp = less_to_compare(members.compare);
	
	while (it1 != last1 and it2 != last2) {
		auto const cmp = comp(*it1, *it2);
		if (cmp < 0) {
			++it1;
		} else if (cmp > 0) {
			++it2;
		} else {
			return set_intersection_pair_iterator(members, it1, it2);
		}
	}
	return set_intersection_pair_iterator(members, last1, last2);
}

template<typename Members, typename ForwardIterator1, typename ForwardIterator2>
struct set_intersection_pair_iterator {
private:
	static constexpr auto max_difference = bounded::min(
		numeric_traits::max_value<iter_difference_t<ForwardIterator1>>,
		numeric_traits::max_value<iter_difference_t<ForwardIterator2>>
	);
public:
	using difference_type = bounded::integer<
		bounded::normalize<-max_difference>,
		bounded::normalize<max_difference>
	>;
	
	constexpr set_intersection_pair_iterator(Members & members, ForwardIterator1 it1, ForwardIterator2 it2):
		m_members(members),
		m_it1(it1),
		m_it2(it2)
	{
	}

	constexpr auto first() const {
		return m_it1;
	}
	constexpr auto second() const {
		return m_it2;
	}
	
	constexpr auto operator+(bounded::constant_t<1>) const {
		return find_first_matching(m_members.get(), containers::next(m_it1), containers::next(m_it2));
	}

	friend constexpr auto operator*(set_intersection_pair_iterator const it) {
		using value_type = std::pair<
			decltype(*bounded::declval<ForwardIterator1>()),
			decltype(*bounded::declval<ForwardIterator2>())
		>;
		return value_type(*it.first(), *it.second());
	}

	// These functions can compare only one pair of iterators because it is not
	// legal to compare iterators from two different ranges. We know that if one
	// set matches, the other must.
	template<
		typename RHSMembers, typename RHSForwardIterator1, typename RHSForwardIterator2
	> requires std::same_as<std::remove_const_t<Members>, std::remove_const_t<RHSMembers>>
	friend constexpr auto operator<=>(
		set_intersection_pair_iterator const lhs,
		set_intersection_pair_iterator<RHSMembers, RHSForwardIterator1, RHSForwardIterator2> const rhs
	) {
		return lhs.first() <=> rhs.first();
	}
	template<
		typename RHSMembers, typename RHSForwardIterator1, typename RHSForwardIterator2
	> requires std::same_as<std::remove_const_t<Members>, std::remove_const_t<RHSMembers>>
	friend constexpr auto operator==(
		set_intersection_pair_iterator const lhs,
		set_intersection_pair_iterator<RHSMembers, RHSForwardIterator1, RHSForwardIterator2> const rhs
	) -> bool {
		return lhs.first() == rhs.first();
	}

private:
	std::reference_wrapper<Members> m_members;	
	ForwardIterator1 m_it1;
	ForwardIterator2 m_it2;
};

// set_intersection_pair is like set_intersection, but it returns both iterators
// TODO: find a better name for this
export template<typename Range1, typename Range2, typename Compare>
struct set_intersection_pair {
	template<typename R1, typename R2>
	constexpr set_intersection_pair(R1 && range1, R2 && range2, Compare compare = Compare()):
		m_members{
			OPERATORS_FORWARD(range1),
			OPERATORS_FORWARD(range2),
			std::move(compare)
		}
	{
		if constexpr (forward_range<Range1>) {
			BOUNDED_ASSERT(is_sorted(m_members.range1, m_members.compare));
		}
		if constexpr (forward_range<Range2>) {
			BOUNDED_ASSERT(is_sorted(m_members.range2, m_members.compare));
		}
	}
	
	constexpr auto begin() const {
		return find_first_matching(
			m_members,
			::containers::begin(m_members.range1),
			::containers::begin(m_members.range2)
		);
	}
	constexpr auto begin() {
		return find_first_matching(
			m_members,
			::containers::begin(m_members.range1),
			::containers::begin(m_members.range2)
		);
	}

	constexpr auto end() const {
		return set_intersection_pair_iterator(m_members, ::containers::end(m_members.range1), ::containers::end(m_members.range2));
	}
	constexpr auto end() {
		return set_intersection_pair_iterator(m_members, ::containers::end(m_members.range1), ::containers::end(m_members.range2));
	}

private:
	set_intersection_members<Range1, Range2, Compare> m_members;
};

template<typename Range1, typename Range2, typename Compare>
set_intersection_pair(Range1 &&, Range2 &&, Compare) -> set_intersection_pair<Range1, Range2, Compare>;

template<typename Range1, typename Range2>
set_intersection_pair(Range1 &&, Range2 &&) -> set_intersection_pair<Range1, Range2, std::compare_three_way>;

} // namespace containers

using namespace bounded::literal;

struct compares_address {
	constexpr explicit compares_address(int const & first, int const & second):
		m_first(first),
		m_second(second)
	{
	}
	constexpr explicit compares_address(std::pair<int const &, int const &> other):
		m_first(other.first),
		m_second(other.second)
	{
	}
	
	friend constexpr auto operator==(compares_address const lhs, compares_address const rhs) -> bool {
		return
			std::addressof(lhs.m_first) == std::addressof(rhs.m_first) and
			std::addressof(lhs.m_second) == std::addressof(rhs.m_second);
	}

	friend constexpr auto operator==(compares_address const lhs, std::pair<int const &, int const &> rhs) -> bool {
		return lhs == compares_address(rhs);
	}

private:
	int const & m_first;
	int const & m_second;
};

using init_list = std::initializer_list<compares_address>;

constexpr auto empty = containers::array<int, 0_bi>{};

static_assert(containers::equal(
	containers::set_intersection_pair(empty, empty),
	init_list{}
));

constexpr auto one_one = containers::array{1};
constexpr auto one_two = containers::array{2};

static_assert(containers::equal(
	containers::set_intersection_pair(empty, one_one),
	init_list{}
));
static_assert(containers::equal(
	containers::set_intersection_pair(empty, one_two),
	init_list{}
));

static_assert(containers::equal(
	containers::set_intersection_pair(one_one, one_one),
	init_list{compares_address(one_one[0_bi], one_one[0_bi])}
));
static_assert(containers::equal(
	containers::set_intersection_pair(one_one, one_two),
	init_list{}
));
static_assert(containers::equal(
	containers::set_intersection_pair(one_two, one_one),
	init_list{}
));
static_assert(containers::equal(
	containers::set_intersection_pair(one_two, one_two),
	init_list{compares_address(one_two[0_bi], one_two[0_bi])}
));

constexpr auto two_forward = containers::array{1, 2};
constexpr auto two_ones = containers::array{1, 1};

static_assert(containers::equal(
	containers::set_intersection_pair(empty, two_forward),
	init_list{}
));

static_assert(containers::equal(
	containers::set_intersection_pair(one_one, two_forward),
	init_list{compares_address(one_one[0_bi], two_forward[0_bi])}
));
static_assert(containers::equal(
	containers::set_intersection_pair(two_forward, one_two),
	init_list{compares_address(two_forward[1_bi], one_two[0_bi])}
));
static_assert(containers::equal(
	containers::set_intersection_pair(two_ones, one_one),
	init_list{compares_address(two_ones[0_bi], one_one[0_bi])}
));

static_assert(containers::equal(
	containers::set_intersection_pair(two_forward, two_forward),
	init_list{
		compares_address(two_forward[0_bi], two_forward[0_bi]),
		compares_address(two_forward[1_bi], two_forward[1_bi])
	}
));
static_assert(containers::equal(
	containers::set_intersection_pair(two_forward, two_ones),
	init_list{compares_address(two_forward[0_bi], two_ones[0_bi])}
));
static_assert(containers::equal(
	containers::set_intersection_pair(two_ones, two_forward),
	init_list{compares_address(two_ones[0_bi], two_forward[0_bi])}
));

constexpr auto all_ones = containers::array{1, 1, 1, 1, 1, 1, 1};
constexpr auto prefix = containers::array{-4, -3, -2, 1, 1, 1};
constexpr auto suffix = containers::array{1, 1, 1, 2, 3, 4};
constexpr auto prefix_suffix = containers::array{-4, -3, -2, 1, 1, 1, 2, 3, 4};

static_assert(containers::equal(
	containers::set_intersection_pair(empty, all_ones),
	init_list{}
));

static_assert(containers::equal(
	containers::set_intersection_pair(one_one, all_ones),
	init_list{compares_address(one_one[0_bi], all_ones[0_bi])}
));
static_assert(containers::equal(
	containers::set_intersection_pair(all_ones, one_one),
	init_list{compares_address(all_ones[0_bi], one_one[0_bi])}
));
static_assert(containers::equal(
	containers::set_intersection_pair(two_ones, all_ones),
	init_list{
		compares_address(two_ones[0_bi], all_ones[0_bi]),
		compares_address(two_ones[1_bi], all_ones[1_bi])
	}
));
static_assert(containers::equal(
	containers::set_intersection_pair(all_ones, two_ones),
	init_list{
		compares_address(all_ones[0_bi], two_ones[0_bi]),
		compares_address(all_ones[1_bi], two_ones[1_bi])
	}
));

static_assert(containers::equal(
	containers::set_intersection_pair(all_ones, prefix),
	init_list{
		compares_address(all_ones[0_bi], prefix[3_bi]),
		compares_address(all_ones[1_bi], prefix[4_bi]),
		compares_address(all_ones[2_bi], prefix[5_bi]),
	}
));
static_assert(containers::equal(
	containers::set_intersection_pair(prefix, all_ones),
	init_list{
		compares_address(prefix[3_bi], all_ones[0_bi]),
		compares_address(prefix[4_bi], all_ones[1_bi]),
		compares_address(prefix[5_bi], all_ones[2_bi]),
	}
));

static_assert(containers::equal(
	containers::set_intersection_pair(all_ones, suffix),
	init_list{
		compares_address(all_ones[0_bi], suffix[0_bi]),
		compares_address(all_ones[1_bi], suffix[1_bi]),
		compares_address(all_ones[2_bi], suffix[2_bi]),
	}
));
static_assert(containers::equal(
	containers::set_intersection_pair(suffix, all_ones),
	init_list{
		compares_address(suffix[0_bi], all_ones[0_bi]),
		compares_address(suffix[1_bi], all_ones[1_bi]),
		compares_address(suffix[2_bi], all_ones[2_bi]),
	}
));

static_assert(containers::equal(
	containers::set_intersection_pair(all_ones, prefix_suffix),
	init_list{
		compares_address(all_ones[0_bi], prefix_suffix[3_bi]),
		compares_address(all_ones[1_bi], prefix_suffix[4_bi]),
		compares_address(all_ones[2_bi], prefix_suffix[5_bi]),
	}
));
static_assert(containers::equal(
	containers::set_intersection_pair(prefix_suffix, all_ones),
	init_list{
		compares_address(prefix_suffix[3_bi], all_ones[0_bi]),
		compares_address(prefix_suffix[4_bi], all_ones[1_bi]),
		compares_address(prefix_suffix[5_bi], all_ones[2_bi]),
	}
));

static_assert(containers::equal(
	containers::set_intersection_pair(suffix, prefix),
	init_list{
		compares_address(suffix[0_bi], prefix[3_bi]),
		compares_address(suffix[1_bi], prefix[4_bi]),
		compares_address(suffix[2_bi], prefix[5_bi]),
	}
));
static_assert(containers::equal(
	containers::set_intersection_pair(prefix, suffix),
	init_list{
		compares_address(prefix[3_bi], suffix[0_bi]),
		compares_address(prefix[4_bi], suffix[1_bi]),
		compares_address(prefix[5_bi], suffix[2_bi]),
	}
));

constexpr auto squares = containers::array{1, 4, 9, 16, 25, 36, 49, 64};
constexpr auto evens = containers::array{2, 4, 6, 8, 10, 12, 14, 16, 18, 20};

static_assert(containers::equal(
	containers::set_intersection_pair(squares, evens),
	init_list{
		compares_address(squares[1_bi], evens[1_bi]),
		compares_address(squares[3_bi], evens[7_bi]),
	}
));
static_assert(containers::equal(
	containers::set_intersection_pair(evens, squares),
	init_list{
		compares_address(evens[1_bi], squares[1_bi]),
		compares_address(evens[7_bi], squares[3_bi]),
	}
));
