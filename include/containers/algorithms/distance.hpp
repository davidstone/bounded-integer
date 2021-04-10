// Copyright David Stone 2016.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/is_iterator_sentinel.hpp>
#include <containers/iter_difference_t.hpp>

#include <bounded/integer.hpp>

#include <iterator>

namespace containers {

using namespace bounded::literal;

template<iterator InputIterator>
constexpr auto distance(InputIterator first, sentinel_for<InputIterator> auto const last) {
	if constexpr (std::is_same_v<typename std::iterator_traits<InputIterator>::iterator_category, std::random_access_iterator_tag>) {
		return last - first;
	} else {
		auto difference = iter_difference_t<InputIterator>(0_bi);
		for (; first != last; ++first) {
			++difference;
		}
		return difference;
	}
}


}	// namespace containers
