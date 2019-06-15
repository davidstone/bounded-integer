// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/algorithms/find.hpp>

#include <bounded/integer.hpp>
#include <bounded/optional.hpp>

#include <type_traits>
#include <utility>

namespace containers {
namespace detail {

template<typename ForwardIterator, typename Sentinel> requires
	std::is_lvalue_reference_v<decltype(*std::declval<ForwardIterator>())>
constexpr auto maybe_find_if_helper(ForwardIterator const it, Sentinel const last) BOUNDED_NOEXCEPT_VALUE(
	it != last ? std::addressof(*it) : nullptr
)

template<typename ForwardIterator, typename Sentinel> requires(
	!std::is_lvalue_reference_v<decltype(*std::declval<ForwardIterator>())>
)
constexpr auto maybe_find_if_helper(ForwardIterator const it, Sentinel const last) BOUNDED_NOEXCEPT_VALUE(
	it != last ? bounded::make_optional(*it) : bounded::none
)

} // namespace detail

// the maybe_find functions return a value contextually convertible to bool. If
// the range contains the element, it can be dereferenced as an optional
template<typename ForwardIterator, typename Sentinel, typename UnaryPredicate>
constexpr auto maybe_find_if(ForwardIterator const first, Sentinel const last, UnaryPredicate p) BOUNDED_NOEXCEPT_VALUE(
	::containers::detail::maybe_find_if_helper(::containers::find_if(first, last, std::move(p)), last)
)

template<typename Range, typename UnaryPredicate>
constexpr auto maybe_find_if(Range && range, UnaryPredicate p) BOUNDED_NOEXCEPT_VALUE(
	::containers::maybe_find_if(begin(BOUNDED_FORWARD(range)), end(BOUNDED_FORWARD(range)), std::move(p))
)


template<typename ForwardIterator, typename Sentinel, typename T>
constexpr auto maybe_find(ForwardIterator const first, Sentinel const last, T const & value) BOUNDED_NOEXCEPT_VALUE(
	::containers::maybe_find_if(first, last, bounded::equal_to(value))
)

template<typename Range, typename T>
constexpr auto maybe_find(Range && range, T const & value) BOUNDED_NOEXCEPT_VALUE(
	::containers::maybe_find(begin(BOUNDED_FORWARD(range)), end(BOUNDED_FORWARD(range)), value)
)

}	// namespace containers
