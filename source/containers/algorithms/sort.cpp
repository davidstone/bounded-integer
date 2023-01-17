// Copyright David Stone 2019.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

export module containers.algorithms.sort;

import containers.algorithms.is_sorted;
import containers.array;
import containers.begin_end;
import containers.is_range;
import containers.legacy_iterator;

import std_module;

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
};
export constexpr auto sort = sort_t();

} // namespace containers

constexpr auto verify_sort(auto && range_) {
	auto impl = [](auto range, auto cmp) {
		containers::sort(range, cmp);
		return containers::is_sorted(range, cmp);
	};
	return impl(range_, std::less()) and impl(range_, std::greater());
}

static_assert(verify_sort(containers::array{1}));

static_assert(verify_sort(containers::array{2, 4}));
static_assert(verify_sort(containers::array{5, 3}));
static_assert(verify_sort(containers::array{1, 1}));

static_assert(verify_sort(containers::array{1, 2, 3}));
static_assert(verify_sort(containers::array{1, 3, 2}));
static_assert(verify_sort(containers::array{2, 1, 3}));
static_assert(verify_sort(containers::array{2, 3, 1}));
static_assert(verify_sort(containers::array{3, 1, 2}));
static_assert(verify_sort(containers::array{3, 2, 1}));

static_assert(verify_sort(containers::array{1, 1, 2}));
static_assert(verify_sort(containers::array{1, 2, 1}));
static_assert(verify_sort(containers::array{2, 1, 1}));
static_assert(verify_sort(containers::array{1, 2, 2}));
static_assert(verify_sort(containers::array{2, 1, 2}));
static_assert(verify_sort(containers::array{2, 2, 1}));
static_assert(verify_sort(containers::array{1, 1, 1}));