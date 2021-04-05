// Copyright David Stone 2021.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/begin_end.hpp>
#include <containers/is_range.hpp>

#include <bounded/detail/construct_destroy.hpp>

namespace containers {

template<iterator InputIterator>
constexpr void destroy_range(InputIterator first, sentinel_for<InputIterator> auto const last) {
	for (; first != last; ++first) {
		bounded::destroy(*first);
	}
}

constexpr void destroy_range(range auto && r) {
	::containers::destroy_range(containers::begin(OPERATORS_FORWARD(r)), containers::end(OPERATORS_FORWARD(r)));
}

} // namespace containers
