// Copyright David Stone 2019.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <containers/algorithms/sort.hpp>
#include <containers/array/array.hpp>
#include <containers/static_vector/static_vector.hpp>

namespace {

template<typename Range>
constexpr auto verify_sort(Range && range) {
	containers::sort(range);
	return containers::is_sorted(range);
}

static_assert(verify_sort(containers::static_vector<int, 1>{}));

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

} // namespace