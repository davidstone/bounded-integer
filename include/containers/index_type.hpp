// Copyright David Stone 2015.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <bounded/integer.hpp>

#include <limits>
#include <type_traits>

namespace containers {

template<typename T>
using index_type = bounded::checked_integer<
	0,
	bounded::detail::normalize<(bounded::max_value<typename std::decay_t<T>::size_type> - bounded::constant<1>).value()>,
	std::out_of_range
>;

}	// namespace containers
