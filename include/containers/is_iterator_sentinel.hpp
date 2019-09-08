// Copyright David Stone 2015.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/is_iterator.hpp>

#include <bounded/integer.hpp>

namespace containers {

template<typename Iterator, typename Sentinel>
concept iterator_sentinel = iterator<Iterator> and bounded::equality_comparable<Iterator, Sentinel>;

}	// namespace containers
