// Copyright David Stone 2015.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/is_iterator.hpp>
#include <containers/type.hpp>

namespace containers {
namespace detail {

template<typename LHS, typename RHS, typename Enable = void>
constexpr auto comparable = false;

template<typename LHS, typename RHS>
constexpr auto comparable<
	LHS,
	RHS,
	void_t<
		decltype(std::declval<LHS const &>() == std::declval<RHS const &>()),
		decltype(std::declval<LHS const &>() != std::declval<RHS const &>())
	>
> = true;

}	// namespace detail

template<typename Iterator, typename Sentinel>
constexpr auto is_iterator_sentinel = is_iterator<Iterator> and detail::comparable<Iterator, Sentinel>;

}	// namespace containers
