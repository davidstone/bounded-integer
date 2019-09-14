// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/begin_end.hpp>
#include <containers/is_range.hpp>
#include <containers/is_iterator_sentinel.hpp>
#include <containers/size.hpp>

#include <bounded/integer.hpp>

#include <utility>

// TODO: All of this can be optimized more

namespace containers {

template<iterator InputIterator1, iterator InputIterator2>
constexpr auto lexicographical_compare_3way(InputIterator1 first1, sentinel_for<InputIterator1> auto const last1, InputIterator2 first2, sentinel_for<InputIterator2> auto const last2, auto cmp) noexcept(
	noexcept(first1 != last1 and first2 != last2) and
	noexcept(++first1) and
	noexcept(++first2) and
	noexcept(cmp(*first1, *first2) != 0)
) {
	for (; first1 != last1 and first2 != last2; ++first1, ++first2) {
		if (auto const result = cmp(*first1, *first2); result != 0) {
			return result;
		}
	}
	return
		(first1 != last1) ? bounded::strong_ordering::greater :
		(first2 != last2) ? bounded::strong_ordering::less :
		bounded::strong_ordering::equal;
}

template<range Range1, range Range2>
constexpr auto lexicographical_compare_3way(Range1 const & range1, Range2 const & range2, auto cmp) BOUNDED_NOEXCEPT_VALUE(
	::containers::lexicographical_compare_3way(begin(range1), end(range1), begin(range2), end(range2), std::move(cmp))
)


template<iterator InputIterator1, iterator InputIterator2>
constexpr auto lexicographical_compare_3way(InputIterator1 const first1, sentinel_for<InputIterator1> auto const last1, InputIterator2 const first2, sentinel_for<InputIterator2> auto const last2) BOUNDED_NOEXCEPT(
	::containers::lexicographical_compare_3way(first1, last1, first2, last2, bounded::compare_3way())
)

template<range Range1, range Range2>
constexpr auto lexicographical_compare_3way(Range1 const & range1, Range2 const & range2) BOUNDED_NOEXCEPT_VALUE(
	::containers::lexicographical_compare_3way(begin(range1), end(range1), begin(range2), end(range2))
)


template<iterator InputIterator1, iterator InputIterator2>
constexpr auto lexicographical_compare_3way(InputIterator1 first1, sentinel_for<InputIterator1> auto const last1, InputIterator2 first2, auto cmp) noexcept(
	noexcept(first1 != last1) and
	noexcept(++first1) and
	noexcept(++first2) and
	noexcept(cmp(*first1, *first2) != 0)
) {
	for (; first1 != last1; ++first1, ++first2) {
		if (auto const result = cmp(*first1, *first2); result != 0) {
			return result;
		}
	}
	return bounded::strong_ordering::equal;
}

template<iterator InputIterator1>
constexpr auto lexicographical_compare_3way(InputIterator1 const first1, sentinel_for<InputIterator1> auto const last1, iterator auto const first2) BOUNDED_NOEXCEPT(
	::containers::lexicographical_compare_3way(first1, last1, first2, bounded::compare_3way())
)




template<iterator InputIterator1, iterator InputIterator2>
constexpr auto equal(InputIterator1 first1, sentinel_for<InputIterator1> auto const last1, InputIterator2 first2, sentinel_for<InputIterator2> auto const last2, auto cmp) noexcept(
	noexcept(first1 != last1 and first2 != last2) and
	noexcept(++first1) and
	noexcept(++first2) and
	noexcept(!cmp(*first1, *first2))
) {
	for (; first1 != last1 and first2 != last2; ++first1, ++first2) {
		if (!cmp(*first1, *first2)) {
			return false;
		}
	}
	return first1 == last1 and first2 == last2;
}

template<range Range1, range Range2>
constexpr auto equal(Range1 const & range1, Range2 const & range2, auto cmp) BOUNDED_NOEXCEPT_VALUE(
	::containers::equal(begin(range1), end(range1), begin(range2), end(range2), std::move(cmp))
)


template<iterator InputIterator1, iterator InputIterator2>
constexpr auto equal(InputIterator1 const first1, sentinel_for<InputIterator1> auto const last1, InputIterator2 const first2, sentinel_for<InputIterator2> auto const last2) BOUNDED_NOEXCEPT(
	::containers::equal(first1, last1, first2, last2, bounded::equal_to())
)

template<range Range1, range Range2>
constexpr auto equal(Range1 const & range1, Range2 const & range2) BOUNDED_NOEXCEPT_VALUE(
	::containers::equal(begin(range1), end(range1), begin(range2), end(range2))
)


template<iterator InputIterator1, iterator InputIterator2>
constexpr auto equal(InputIterator1 first1, sentinel_for<InputIterator1> auto const last1, InputIterator2 first2, auto cmp) noexcept(
	noexcept(first1 != last1) and
	noexcept(++first1) and
	noexcept(++first2) and
	noexcept(!cmp(*first1, *first2))
) {
	for (; first1 != last1; ++first1, ++first2) {
		if (!cmp(*first1, *first2)) {
			return false;
		}
	}
	return first1 == last1;
}

template<iterator InputIterator1, iterator InputIterator2>
constexpr auto equal(InputIterator1 const first1, sentinel_for<InputIterator1> auto const last1, InputIterator2 const first2) BOUNDED_NOEXCEPT(
	::containers::equal(first1, last1, first2, bounded::equal_to())
)


namespace detail {
namespace common {

template<range T>
constexpr auto compare(T const & lhs, T const & rhs) BOUNDED_NOEXCEPT(
	::containers::lexicographical_compare_3way(
		begin(std::as_const(lhs)), end(std::as_const(lhs)),
		begin(std::as_const(rhs)), end(std::as_const(rhs))
	)
)

template<range T>
constexpr auto operator==(T const & lhs, T const & rhs) BOUNDED_NOEXCEPT(
	size(lhs) == size(rhs) and ::containers::equal(
		begin(std::as_const(lhs)), end(std::as_const(lhs)),
		begin(std::as_const(rhs))
	)
)

}	// namespace common

using ::containers::detail::common::compare;
using ::containers::detail::common::operator==;

}	// namespace detail

using ::containers::detail::common::compare;
using ::containers::detail::common::operator==;

}	// namespace containers
