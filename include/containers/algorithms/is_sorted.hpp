// Copyright David Stone 2019.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/algorithms/advance.hpp>
#include <containers/begin_end.hpp>
#include <containers/is_range.hpp>

#include <functional>

namespace containers {

struct is_sorted_t {
	constexpr bool operator()(range auto && to_sort, auto cmp) const {
		auto first = containers::begin(to_sort);
		auto const last = containers::end(to_sort);
		if (first != last) {
			for (auto next = containers::next(first); next != last; ++next) {
				// TODO: Handle this issue generically
				if constexpr (std::is_same_v<decltype(cmp(*next, *first)), bool>) {
					if (cmp(*next, *first)) {
						return false;
					}
				} else {
					if (cmp(*next, *first) < 0) {
						return false;
					}
				}
				first = next;
			}
		}
		return true;
	}
	constexpr bool operator()(range auto && to_sort) const {
		return operator()(to_sort, std::less());
	}
} constexpr inline is_sorted;

} // namespace containers
