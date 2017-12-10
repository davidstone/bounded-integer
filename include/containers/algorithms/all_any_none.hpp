// Copyright David Stone 2015.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/algorithms/find.hpp>

#include <bounded/integer.hpp>

#include <type_traits>

namespace containers {

template<typename InputIterator, typename Sentinel, typename UnaryPredicate>
constexpr auto all_of(InputIterator const first, Sentinel const last, UnaryPredicate p) BOUNDED_NOEXCEPT_VALUE(
	::containers::find_if_not(first, last, p) == last
)

template<typename InputIterator, typename Sentinel, typename UnaryPredicate>
constexpr auto any_of(InputIterator const first, Sentinel const last, UnaryPredicate p) BOUNDED_NOEXCEPT_VALUE(
	::containers::find_if(first, last, p) != last
)

template<typename InputIterator, typename Sentinel, typename UnaryPredicate>
constexpr auto none_of(InputIterator const first, Sentinel const last, UnaryPredicate p) BOUNDED_NOEXCEPT_VALUE(
	::containers::find_if(first, last, p) == last
)

}	// namespace containers
