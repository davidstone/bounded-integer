// Copyright David Stone 2015.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/is_iterator.hpp>

#include <bounded/integer.hpp>

namespace containers {

template<typename Sentinel, typename Iterator>
concept sentinel_for = iterator<Iterator> and bounded::equality_comparable<Iterator, Sentinel>;

template<typename Sentinel, typename Iterator>
concept random_access_sentinel_for = sentinel_for<Sentinel, Iterator> and requires(Iterator first, Sentinel last) { last - first; };

}	// namespace containers
