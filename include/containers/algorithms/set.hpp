// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/algorithms/advance.hpp>
#include <containers/algorithms/is_sorted.hpp>
#include <containers/begin_end.hpp>
#include <containers/iter_difference_t.hpp>
#include <containers/iter_value_t.hpp>

#include <bounded/concepts.hpp>

#include <numeric_traits/min_max_value.hpp>

#include <operators/forward.hpp>

#include <functional>
#include <iterator>
#include <type_traits>
#include <utility>

namespace containers {
namespace detail {

constexpr auto less_to_compare = [](auto const & cmp) {
	return [&](auto const & lhs, auto const & rhs) {
		using result_t = decltype(cmp(lhs, rhs));
		if constexpr (std::is_convertible_v<result_t, std::strong_ordering>) {
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
	auto const comp = ::containers::detail::less_to_compare(members.compare);
	
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
	using value_type = std::pair<
		decltype(*std::declval<ForwardIterator1>()),
		decltype(*std::declval<ForwardIterator2>())
	>;
	using difference_type = bounded::integer<
		bounded::normalize<-max_difference>,
		bounded::normalize<max_difference>
	>;
	using pointer = value_type *;
	using reference = value_type;
	using iterator_category = std::forward_iterator_tag;
	
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

private:
	std::reference_wrapper<Members> m_members;	
	ForwardIterator1 m_it1;
	ForwardIterator2 m_it2;
};

template<typename Members, typename ForwardIterator1, typename ForwardIterator2>
constexpr auto operator*(set_intersection_pair_iterator<Members, ForwardIterator1, ForwardIterator2> const it) {
	return typename set_intersection_pair_iterator<Members, ForwardIterator1, ForwardIterator2>::value_type(*it.first(), *it.second());
}

// These functions can compare only one pair of iterators because it is not
// legal to compare iterators from two different ranges. We know that if one
// set matches, the other must.
template<
	typename LHSMembers, typename LHSForwardIterator1, typename LHSForwardIterator2,
	typename RHSMembers, typename RHSForwardIterator1, typename RHSForwardIterator2
> requires (std::is_same_v<std::remove_const_t<LHSMembers>, std::remove_const_t<RHSMembers>>)
constexpr auto operator<=>(
	set_intersection_pair_iterator<LHSMembers, LHSForwardIterator1, LHSForwardIterator2> const lhs,
	set_intersection_pair_iterator<RHSMembers, RHSForwardIterator1, RHSForwardIterator2> const rhs
) {
	return lhs.first() <=> rhs.first();
}
template<
	typename LHSMembers, typename LHSForwardIterator1, typename LHSForwardIterator2,
	typename RHSMembers, typename RHSForwardIterator1, typename RHSForwardIterator2
> requires (std::is_same_v<std::remove_const_t<LHSMembers>, std::remove_const_t<RHSMembers>>)
constexpr auto operator==(
	set_intersection_pair_iterator<LHSMembers, LHSForwardIterator1, LHSForwardIterator2> const lhs,
	set_intersection_pair_iterator<RHSMembers, RHSForwardIterator1, RHSForwardIterator2> const rhs
) -> bool {
	return lhs.first() == rhs.first();
}

} // namespace detail

// set_intersection_pair is like set_intersection, but it returns both iterators
// TODO: find a better name for this
template<typename Range1, typename Range2, typename Compare>
struct set_intersection_pair {
private:
	using members_t = detail::set_intersection_members<Range1, Range2, Compare>;
public:
	using iterator = detail::set_intersection_pair_iterator<
		members_t,
		decltype(containers::begin(std::declval<Range1 &>())),
		decltype(containers::begin(std::declval<Range2 &>()))
	>;
	using const_iterator = detail::set_intersection_pair_iterator<
		members_t const,
		decltype(containers::begin(std::declval<Range1 const &>())),
		decltype(containers::begin(std::declval<Range2 const &>()))
	>;
	using value_type = iter_value_t<iterator>;

	template<typename R1, typename R2>
	constexpr set_intersection_pair(R1 && range1, R2 && range2, Compare compare = Compare()):
		m_members{
			OPERATORS_FORWARD(range1),
			OPERATORS_FORWARD(range2),
			std::move(compare)
		}
	{
	}
	
	constexpr auto begin() const & {
		return find_first_matching(
			m_members,
			::containers::begin(m_members.range1),
			::containers::begin(m_members.range2)
		);
	}
	constexpr auto begin() & {
		return find_first_matching(
			m_members,
			::containers::begin(m_members.range1),
			::containers::begin(m_members.range2)
		);
	}

	constexpr auto end() const & {
		return const_iterator(m_members, ::containers::end(m_members.range1), ::containers::end(m_members.range2));
	}
	constexpr auto end() & {
		return iterator(m_members, ::containers::end(m_members.range1), ::containers::end(m_members.range2));
	}

private:
	members_t m_members;
};

template<typename Range1, typename Range2, typename Compare>
set_intersection_pair(Range1 &&, Range2 &&, Compare) -> set_intersection_pair<Range1, Range2, Compare>;

template<typename Range1, typename Range2>
set_intersection_pair(Range1 &&, Range2 &&) -> set_intersection_pair<Range1, Range2, bounded::detail::compare_to_t>;

}	// namespace containers
