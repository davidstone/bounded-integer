// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/begin_end.hpp>
#include <containers/is_iterator_sentinel.hpp>

#include <iterator>
#include <type_traits>

namespace containers {

using std::begin;
using std::end;

template<typename T>
concept range = sentinel_for<decltype(containers::end(std::declval<T const &>())), decltype(containers::begin(std::declval<T const &>()))>;

}	// namespace containers
