// Copyright David Stone 2016.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/add_remove_const.hpp>
#include <containers/begin_end.hpp>
#include <containers/is_iterable.hpp>
#include <containers/is_iterator_sentinel.hpp>

#include <bounded/integer.hpp>

namespace containers {
namespace detail {

using bounded::compare;
constexpr auto compare_to = [](auto const & lhs, auto const & rhs) BOUNDED_NOEXCEPT_VALUE(
	compare(lhs, rhs)
);

}	// namespace detail

template<
	typename InputIterator1, typename Sentinel1,
	typename InputIterator2, typename Sentinel2,
	typename BinaryPredicate,
	BOUNDED_REQUIRES(is_iterator_sentinel<InputIterator1, Sentinel1> and is_iterator_sentinel<InputIterator2, Sentinel2>)
>
constexpr auto compare(InputIterator1 first1, Sentinel1 const last1, InputIterator2 first2, Sentinel2 const last2, BinaryPredicate cmp) noexcept(
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
		(first1 != last1) ? bounded::strong_ordering_greater :
		(first2 != last2) ? bounded::strong_ordering_less :
		bounded::strong_ordering_equal;
}

template<
	typename InputIterator1, typename Sentinel1,
	typename InputIterator2, typename Sentinel2,
	BOUNDED_REQUIRES(is_iterator_sentinel<InputIterator1, Sentinel1> and is_iterator_sentinel<InputIterator2, Sentinel2>)
>
constexpr auto compare(InputIterator1 const first1, Sentinel1 const last1, InputIterator2 const first2, Sentinel2 const last2) BOUNDED_NOEXCEPT(
	::containers::compare(first1, last1, first2, last2, detail::compare_to)
)


template<
	typename InputIterator1, typename Sentinel1,
	typename InputIterator2,
	typename BinaryPredicate,
	BOUNDED_REQUIRES(is_iterator_sentinel<InputIterator1, Sentinel1> and is_iterator<InputIterator2> and !is_iterator_sentinel<InputIterator2, BinaryPredicate>)
>
constexpr auto compare(InputIterator1 first1, Sentinel1 const last1, InputIterator2 first2, BinaryPredicate cmp) noexcept(
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
	return bounded::strong_ordering_equal;
}

template<
	typename InputIterator1, typename Sentinel1,
	typename InputIterator2,
	BOUNDED_REQUIRES(is_iterator_sentinel<InputIterator1, Sentinel1> and is_iterator<InputIterator2>)
>
constexpr auto compare(InputIterator1 const first1, Sentinel1 const last1, InputIterator2 const first2) BOUNDED_NOEXCEPT(
	::containers::compare(first1, last1, first2, detail::compare_to)
)


namespace detail {

// TODO: Define orderings other than strong_ordering
constexpr auto equal_to_compare = [](auto cmp) noexcept {
	return [cmp = std::move(cmp)](auto const & lhs, auto const & rhs) BOUNDED_NOEXCEPT_DECLTYPE(
		cmp(lhs, rhs) ? bounded::strong_ordering_equal : bounded::strong_ordering_less
	);
};

}	// namespace detail


template<
	typename InputIterator1, typename Sentinel1,
	typename InputIterator2, typename Sentinel2,
	typename BinaryPredicate,
	BOUNDED_REQUIRES(is_iterator_sentinel<InputIterator1, Sentinel1> and is_iterator_sentinel<InputIterator2, Sentinel2>)
>
constexpr auto equal(InputIterator1 first1, Sentinel1 const last1, InputIterator2 first2, Sentinel2 const last2, BinaryPredicate eq) BOUNDED_NOEXCEPT(
	::containers::compare(first1, last1, first2, last2, detail::equal_to_compare(eq)) == 0
)

template<
	typename InputIterator1, typename Sentinel1,
	typename InputIterator2, typename Sentinel2
>
constexpr auto equal(InputIterator1 const first1, Sentinel1 const last1, InputIterator2 const first2, Sentinel2 const last2) BOUNDED_NOEXCEPT(
	::containers::compare(first1, last1, first2, last2) == 0
)


template<
	typename InputIterator1, typename Sentinel1,
	typename InputIterator2,
	typename BinaryPredicate,
	BOUNDED_REQUIRES(is_iterator_sentinel<InputIterator1, Sentinel1> and is_iterator<InputIterator2> and !is_iterator_sentinel<InputIterator2, BinaryPredicate>)
>
constexpr auto equal(InputIterator1 first1, Sentinel1 const last1, InputIterator2 first2, BinaryPredicate eq) BOUNDED_NOEXCEPT_DECLTYPE(
	::containers::compare(first1, last1, first2, detail::equal_to_compare(eq)) == 0
)

template<
	typename InputIterator1, typename Sentinel1,
	typename InputIterator2,
	BOUNDED_REQUIRES(is_iterator_sentinel<InputIterator1, Sentinel1> and is_iterator<InputIterator2>)
>
constexpr auto equal(InputIterator1 const first1, Sentinel1 const last1, InputIterator2 const first2) BOUNDED_NOEXCEPT(
	::containers::compare(first1, last1, first2) == 0
)


namespace detail {
namespace common {

// Rather than just accepting Iterable const &, we do this to work around
// https://bugs.llvm.org/show_bug.cgi?id=32860
template<typename LHS, typename RHS, BOUNDED_REQUIRES(
	std::is_same<std::remove_cv_t<std::remove_reference_t<LHS>>, std::remove_cv_t<std::remove_reference_t<RHS>>>{} and
	is_iterable<std::remove_cv_t<std::remove_reference_t<LHS>>>
)>
constexpr auto compare(LHS && lhs, RHS && rhs) BOUNDED_NOEXCEPT(
	::containers::compare(
		begin(::containers::detail::add_const(lhs)), end(::containers::detail::add_const(lhs)),
		begin(::containers::detail::add_const(rhs)), end(::containers::detail::add_const(rhs))
	)
)

}	// namespace common

using ::containers::detail::common::compare;

}	// namespace detail

using ::containers::detail::common::compare;

}	// namespace containers
