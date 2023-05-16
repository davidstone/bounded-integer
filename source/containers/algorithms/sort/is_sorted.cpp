// Copyright David Stone 2019.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

export module containers.algorithms.sort.is_sorted;

import containers.algorithms.advance;
import containers.array;
import containers.begin_end;
import containers.is_range;

import bounded;
import std_module;

using namespace bounded::literal;

namespace containers {

struct is_sorted_t {
	static constexpr bool operator()(range auto && to_sort, auto cmp) {
		auto first = containers::begin(to_sort);
		auto const last = containers::end(to_sort);
		if (first != last) {
			for (auto next = containers::next(first); next != last; ++next) {
				// TODO: Handle this issue generically
				if constexpr (std::same_as<decltype(cmp(*next, *first)), bool>) {
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
	static constexpr bool operator()(range auto && to_sort) {
		return operator()(to_sort, std::less());
	}
};
export constexpr auto is_sorted = is_sorted_t();

} // namespace containers

static_assert(containers::is_sorted(containers::array<int, 0_bi>()));
static_assert(containers::is_sorted(containers::array({1})));
static_assert(containers::is_sorted(containers::array({1, 2})));
static_assert(containers::is_sorted(containers::array({1, 1})));
static_assert(!containers::is_sorted(containers::array({2, 1})));
static_assert(containers::is_sorted(containers::array({1, 2, 3})));
static_assert(containers::is_sorted(containers::array({1, 1, 2})));
static_assert(!containers::is_sorted(containers::array({1, 3, 2})));
static_assert(!containers::is_sorted(containers::array({1, 2, 1})));
static_assert(!containers::is_sorted(containers::array({1, 1, 0})));