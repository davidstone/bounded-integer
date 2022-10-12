// Copyright David Stone 2019.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/begin_end.hpp>
#include <containers/is_range.hpp>
#include <containers/legacy_iterator.hpp>

#include <algorithm>
#include <functional>

namespace containers {

struct sort_t {
	static constexpr auto operator()(range auto & to_sort, auto cmp) -> void {
		std::sort(
			make_legacy_iterator(containers::begin(to_sort)),
			make_legacy_iterator(containers::end(to_sort)),
			cmp
		);
	}
	static constexpr auto operator()(range auto & to_sort) -> void {
		operator()(to_sort, std::less());
	}
} constexpr inline sort;

} // namespace containers
