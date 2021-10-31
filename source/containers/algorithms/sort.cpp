// Copyright David Stone 2019.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <containers/algorithms/sort.hpp>
#include <containers/algorithms/is_sorted.hpp>
#include <containers/array.hpp>

namespace {

constexpr auto verify_sort(auto && range_) {
	auto impl = [](auto range, auto cmp) {
		containers::sort(range, cmp);
		return containers::is_sorted(range, cmp);
	};
	return impl(range_, std::less{}) and impl(range_, std::greater{});
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

} // namespace