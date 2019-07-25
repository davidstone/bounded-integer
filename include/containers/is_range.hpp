// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/is_iterator_sentinel.hpp>

#include <iterator>

namespace containers {

using std::begin;
using std::end;

template<typename T, typename = void>
inline constexpr auto is_range = false;

// This has to be slightly more complicated than I would like due to
// https://stackoverflow.com/questions/22486386/partially-specializing-a-non-type-template-parameter-of-dependent-type
template<typename T>
inline constexpr auto is_range<
	T,
	std::void_t<decltype(begin(std::declval<T>())), decltype(end(std::declval<T>()))>
> = is_iterator_sentinel<decltype(begin(std::declval<T>())), decltype(end(std::declval<T>()))>;

}	// namespace containers
