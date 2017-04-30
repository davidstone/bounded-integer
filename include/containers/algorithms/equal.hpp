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

template<
	typename InputIterator1, typename Sentinel1,
	typename InputIterator2, typename Sentinel2,
	typename BinaryPredicate,
	BOUNDED_REQUIRES(is_iterator_sentinel<InputIterator1, Sentinel1> and is_iterator_sentinel<InputIterator2, Sentinel2>)
>
constexpr auto equal(InputIterator1 first1, Sentinel1 const last1, InputIterator2 first2, Sentinel2 const last2, BinaryPredicate compare) noexcept(
	noexcept(first1 != last1 and first2 != last2) and
	noexcept(++first1) and
	noexcept(++first2) and
	noexcept(!compare(*first1, *first2))
) {
	for (; first1 != last1 and first2 != last2; ++first1, ++first2) {
		if (!compare(*first1, *first2)) {
			return false;
		}
	}
	return first1 == last1 and first2 == last2;
}

template<
	typename InputIterator1, typename Sentinel1,
	typename InputIterator2, typename Sentinel2,
	BOUNDED_REQUIRES(is_iterator_sentinel<InputIterator1, Sentinel1> and is_iterator_sentinel<InputIterator2, Sentinel2>)
>
constexpr auto equal(InputIterator1 const first1, Sentinel1 const last1, InputIterator2 const first2, Sentinel2 const last2) BOUNDED_NOEXCEPT(
	::containers::equal(first1, last1, first2, last2, bounded::equal_to())
)


template<
	typename InputIterator1, typename Sentinel1,
	typename InputIterator2,
	typename BinaryPredicate,
	BOUNDED_REQUIRES(is_iterator_sentinel<InputIterator1, Sentinel1> and is_iterator<InputIterator2> and !is_iterator_sentinel<InputIterator2, BinaryPredicate>)
>
constexpr auto equal(InputIterator1 first1, Sentinel1 const last1, InputIterator2 first2, BinaryPredicate compare) noexcept(
	noexcept(first1 != last1) and
	noexcept(++first1) and
	noexcept(++first2) and
	noexcept(!compare(*first1, *first2))
) {
	for (; first1 != last1; ++first1, ++first2) {
		if (!compare(*first1, *first2)) {
			return false;
		}
	}
	return true;
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
	is_iterable<std::remove_cv_t<std::remove_reference_t<LHS>>>
)>
constexpr auto operator==(LHS && lhs, RHS && rhs) BOUNDED_NOEXCEPT(
	::containers::equal(
		begin(::containers::detail::add_const(lhs)), end(::containers::detail::add_const(lhs)),
		begin(::containers::detail::add_const(rhs)), end(::containers::detail::add_const(rhs))
	)
)

}	// namespace common

using ::containers::detail::common::operator==;

}	// namespace detail

using ::containers::detail::common::operator==;

}	// namespace containers
