// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/begin_end.hpp>
#include <containers/is_range.hpp>
#include <containers/is_iterator_sentinel.hpp>

#include <bounded/integer.hpp>

#include <utility>

// TODO: All of this can be optimized more

namespace containers {

template<
	typename InputIterator1, typename Sentinel1,
	typename InputIterator2, typename Sentinel2,
	typename BinaryPredicate,
	BOUNDED_REQUIRES(is_iterator_sentinel<InputIterator1, Sentinel1> and is_iterator_sentinel<InputIterator2, Sentinel2>)
>
constexpr auto lexicographical_compare_3way(InputIterator1 first1, Sentinel1 const last1, InputIterator2 first2, Sentinel2 const last2, BinaryPredicate cmp) noexcept(
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
		(first1 != last1) ? std::strong_ordering::greater :
		(first2 != last2) ? std::strong_ordering::less :
		std::strong_ordering::equal;
}

template<typename Range1, typename Range2, typename BinaryPredicate, BOUNDED_REQUIRES(is_range<Range1> and is_range<Range2>)>
constexpr auto lexicographical_compare_3way(Range1 const & range1, Range2 const & range2, BinaryPredicate cmp) BOUNDED_NOEXCEPT_VALUE(
	::containers::lexicographical_compare_3way(begin(range1), end(range1), begin(range2), end(range2), std::move(cmp))
)


template<
	typename InputIterator1, typename Sentinel1,
	typename InputIterator2, typename Sentinel2,
	BOUNDED_REQUIRES(is_iterator_sentinel<InputIterator1, Sentinel1> and is_iterator_sentinel<InputIterator2, Sentinel2>)
>
constexpr auto lexicographical_compare_3way(InputIterator1 const first1, Sentinel1 const last1, InputIterator2 const first2, Sentinel2 const last2) BOUNDED_NOEXCEPT(
	::containers::lexicographical_compare_3way(first1, last1, first2, last2, bounded::compare_3way())
)

template<typename Range1, typename Range2, BOUNDED_REQUIRES(is_range<Range1> and is_range<Range2>)>
constexpr auto lexicographical_compare_3way(Range1 const & range1, Range2 const & range2) BOUNDED_NOEXCEPT_VALUE(
	::containers::lexicographical_compare_3way(begin(range1), end(range1), begin(range2), end(range2))
)


template<
	typename InputIterator1, typename Sentinel1,
	typename InputIterator2,
	typename BinaryPredicate,
	BOUNDED_REQUIRES(is_iterator_sentinel<InputIterator1, Sentinel1> and is_iterator<InputIterator2> and !is_iterator_sentinel<InputIterator2, BinaryPredicate>)
>
constexpr auto lexicographical_compare_3way(InputIterator1 first1, Sentinel1 const last1, InputIterator2 first2, BinaryPredicate cmp) noexcept(
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
	return std::strong_ordering::equal;
}

template<
	typename InputIterator1, typename Sentinel1,
	typename InputIterator2,
	BOUNDED_REQUIRES(is_iterator_sentinel<InputIterator1, Sentinel1> and is_iterator<InputIterator2>)
>
constexpr auto lexicographical_compare_3way(InputIterator1 const first1, Sentinel1 const last1, InputIterator2 const first2) BOUNDED_NOEXCEPT(
	::containers::lexicographical_compare_3way(first1, last1, first2, bounded::compare_3way())
)




template<
	typename InputIterator1, typename Sentinel1,
	typename InputIterator2, typename Sentinel2,
	typename BinaryPredicate,
	BOUNDED_REQUIRES(is_iterator_sentinel<InputIterator1, Sentinel1> and is_iterator_sentinel<InputIterator2, Sentinel2>)
>
constexpr auto equal(InputIterator1 first1, Sentinel1 const last1, InputIterator2 first2, Sentinel2 const last2, BinaryPredicate cmp) noexcept(
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

template<typename Range1, typename Range2, typename BinaryPredicate, BOUNDED_REQUIRES(is_range<Range1> and is_range<Range2>)>
constexpr auto equal(Range1 const & range1, Range2 const & range2, BinaryPredicate cmp) BOUNDED_NOEXCEPT_VALUE(
	::containers::equal(begin(range1), end(range1), begin(range2), end(range2), std::move(cmp))
)


template<
	typename InputIterator1, typename Sentinel1,
	typename InputIterator2, typename Sentinel2,
	BOUNDED_REQUIRES(is_iterator_sentinel<InputIterator1, Sentinel1> and is_iterator_sentinel<InputIterator2, Sentinel2>)
>
constexpr auto equal(InputIterator1 const first1, Sentinel1 const last1, InputIterator2 const first2, Sentinel2 const last2) BOUNDED_NOEXCEPT(
	::containers::equal(first1, last1, first2, last2, bounded::equal_to())
)

template<typename Range1, typename Range2, BOUNDED_REQUIRES(is_range<Range1> and is_range<Range2>)>
constexpr auto equal(Range1 const & range1, Range2 const & range2) BOUNDED_NOEXCEPT_VALUE(
	::containers::equal(begin(range1), end(range1), begin(range2), end(range2))
)


template<
	typename InputIterator1, typename Sentinel1,
	typename InputIterator2,
	typename BinaryPredicate,
	BOUNDED_REQUIRES(is_iterator_sentinel<InputIterator1, Sentinel1> and is_iterator<InputIterator2> and !is_iterator_sentinel<InputIterator2, BinaryPredicate>)
>
constexpr auto equal(InputIterator1 first1, Sentinel1 const last1, InputIterator2 first2, BinaryPredicate cmp) noexcept(
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

template<
	typename InputIterator1, typename Sentinel1,
	typename InputIterator2,
	BOUNDED_REQUIRES(is_iterator_sentinel<InputIterator1, Sentinel1> and is_iterator<InputIterator2>)
>
constexpr auto equal(InputIterator1 const first1, Sentinel1 const last1, InputIterator2 const first2) BOUNDED_NOEXCEPT(
	::containers::equal(first1, last1, first2, bounded::equal_to())
)


namespace detail {
namespace common {

// Rather than just accepting Iterable const &, we do this to work around
// https://bugs.llvm.org/show_bug.cgi?id=32860
template<typename LHS, typename RHS, BOUNDED_REQUIRES(
	std::is_same<std::remove_cv_t<std::remove_reference_t<LHS>>, std::remove_cv_t<std::remove_reference_t<RHS>>>{} and
	is_range<std::remove_cv_t<std::remove_reference_t<LHS>>>
)>
constexpr auto operator<=>(LHS && lhs, RHS && rhs) BOUNDED_NOEXCEPT(
	::containers::lexicographical_compare_3way(
		begin(std::as_const(lhs)), end(std::as_const(lhs)),
		begin(std::as_const(rhs)), end(std::as_const(rhs))
	)
)

template<typename LHS, typename RHS, BOUNDED_REQUIRES(
	std::is_same_v<
		std::remove_cv_t<std::remove_reference_t<LHS>>,
		std::remove_cv_t<std::remove_reference_t<RHS>>
	> and
	is_range<std::remove_cv_t<std::remove_reference_t<LHS>>>
)>
constexpr auto operator==(LHS && lhs, RHS && rhs) BOUNDED_NOEXCEPT(
	size(lhs) == size(rhs) and ::containers::equal(
		begin(std::as_const(lhs)), end(std::as_const(lhs)),
		begin(std::as_const(rhs))
	)
)

}	// namespace common

using ::containers::detail::common::operator<=>;
using ::containers::detail::common::operator==;

}	// namespace detail

using ::containers::detail::common::operator<=>;
using ::containers::detail::common::operator==;

}	// namespace containers
