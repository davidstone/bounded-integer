// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/algorithms/advance.hpp>
#include <containers/begin_end.hpp>
#include <containers/reference_wrapper.hpp>

#include <operators/forward.hpp>
#include <bounded/concepts.hpp>

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

template<typename Range, typename ForwardIterator1, typename ForwardIterator2>
struct set_intersection_pair_iterator {
private:
	static constexpr auto max_difference = bounded::min(
		bounded::max_value<typename std::iterator_traits<ForwardIterator1>::difference_type>,
		bounded::max_value<typename std::iterator_traits<ForwardIterator2>::difference_type>
	);
public:
	using value_type = std::pair<
		decltype(*std::declval<ForwardIterator1>()),
		decltype(*std::declval<ForwardIterator2>())
	>;
	using difference_type = bounded::integer<
		bounded::detail::normalize<static_cast<std::intmax_t>(-max_difference)>,
		bounded::detail::normalize<static_cast<std::intmax_t>(max_difference)>
	>;
	using pointer = value_type *;
	using reference = value_type;
	using iterator_category = std::forward_iterator_tag;
	
	constexpr set_intersection_pair_iterator(Range & range, ForwardIterator1 it1, ForwardIterator2 it2):
		m_range(range),
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
		return Range::find_first_matching(m_range.get(), containers::next(m_it1), containers::next(m_it2));
	}

private:
	reference_wrapper<Range> m_range;	
	ForwardIterator1 m_it1;
	ForwardIterator2 m_it2;
};

template<typename Range, typename ForwardIterator1, typename ForwardIterator2>
constexpr auto operator*(set_intersection_pair_iterator<Range, ForwardIterator1, ForwardIterator2> const it) {
	return typename set_intersection_pair_iterator<Range, ForwardIterator1, ForwardIterator2>::value_type(*it.first(), *it.second());
}

// These functions can compare only one pair of iterators because it is not
// legal to compare iterators from two different ranges. We know that if one
// set matches, the other must.
template<
	typename LHSForwardIterator1, typename LHSForwardIterator2, typename LHSRange,
	typename RHSForwardIterator1, typename RHSForwardIterator2, typename RHSRange
> requires (std::is_same_v<std::remove_const_t<LHSRange>, std::remove_const_t<RHSRange>>)
constexpr auto operator<=>(
	set_intersection_pair_iterator<LHSForwardIterator1, LHSForwardIterator2, LHSRange> const lhs,
	set_intersection_pair_iterator<RHSForwardIterator1, RHSForwardIterator2, RHSRange> const rhs
) {
	return lhs.first() <=> rhs.first();
}
template<
	typename LHSForwardIterator1, typename LHSForwardIterator2, typename LHSRange,
	typename RHSForwardIterator1, typename RHSForwardIterator2, typename RHSRange
> requires (std::is_same_v<std::remove_const_t<LHSRange>, std::remove_const_t<RHSRange>>)
constexpr auto operator==(
	set_intersection_pair_iterator<LHSForwardIterator1, LHSForwardIterator2, LHSRange> const lhs,
	set_intersection_pair_iterator<RHSForwardIterator1, RHSForwardIterator2, RHSRange> const rhs
) -> bool {
	return lhs.first() == rhs.first();
}

} // namespace detail

// set_intersection_pair is like set_intersection, but it returns both iterators
// TODO: find a better name for this
template<typename Range1, typename Range2, typename Compare>
struct set_intersection_pair {
	using iterator = detail::set_intersection_pair_iterator<
		set_intersection_pair<Range1, Range2, Compare>,
		decltype(begin(std::declval<Range1 &>())),
		decltype(begin(std::declval<Range2 &>()))
	>;
	using const_iterator = detail::set_intersection_pair_iterator<
		set_intersection_pair<Range1, Range2, Compare> const,
		decltype(begin(std::declval<Range1 const &>())),
		decltype(begin(std::declval<Range2 const &>()))
	>;
	using value_type = decltype(*std::declval<iterator>());
	using size_type = bounded::integer<
		0,
		bounded::detail::builtin_max_value<typename iterator::difference_type>
	>;

	template<typename R1, typename R2>
	constexpr set_intersection_pair(R1 && range1, R2 && range2, Compare compare):
		m_range1(OPERATORS_FORWARD(range1)),
		m_range2(OPERATORS_FORWARD(range2)),
		m_compare(std::move(compare))
	{
	}
	
	template<typename R1, typename R2>
	constexpr set_intersection_pair(R1 && range1, R2 && range2):
		m_range1(OPERATORS_FORWARD(range1)),
		m_range2(OPERATORS_FORWARD(range2))
	{
	}
	
	constexpr auto begin() const & {
		return find_first_matching(
			*this,
			::containers::begin(m_range1),
			::containers::begin(m_range2)
		);
	}
	constexpr auto begin() & {
		return find_first_matching(
			*this,
			::containers::begin(m_range1),
			::containers::begin(m_range2)
		);
	}

	constexpr auto end() const & {
		return const_iterator(*this, ::containers::end(m_range1), ::containers::end(m_range2));
	}
	constexpr auto end() & {
		return iterator(*this, ::containers::end(m_range1), ::containers::end(m_range2));
	}

private:
	friend iterator;
	friend const_iterator;

	static constexpr auto find_first_matching(auto & self, auto it1, auto it2) {
		auto const last1 = ::containers::end(self.m_range1);
		auto const last2 = ::containers::end(self.m_range2);
		auto const comp = detail::less_to_compare(self.m_compare);
		
		while (it1 != last1 and it2 != last2) {
			auto const cmp = comp(*it1, *it2);
			if (cmp < 0) {
				++it1;
			} else if (cmp > 0) {
				++it2;
			} else {
				return detail::set_intersection_pair_iterator(self, it1, it2);
			}
		}
		return detail::set_intersection_pair_iterator(self, last1, last2);
	}

	Range1 m_range1;
	Range2 m_range2;
	Compare m_compare;
};

template<typename Range1, typename Range2, typename Compare>
set_intersection_pair(Range1 &&, Range2 &&, Compare) -> set_intersection_pair<Range1, Range2, Compare>;

template<typename Range1, typename Range2>
set_intersection_pair(Range1 &&, Range2 &&) -> set_intersection_pair<Range1, Range2, bounded::detail::compare_to_t>;

}	// namespace containers
