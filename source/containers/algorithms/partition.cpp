// Copyright David Stone 2019.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <containers/algorithms/partition.hpp>
#include <containers/array/array.hpp>
#include <containers/static_vector/static_vector.hpp>

namespace {

constexpr auto predicate = [](bool const value) { return value; };

static_assert(containers::is_partitioned(containers::static_vector<bool, 1>{}, predicate));

static_assert(containers::is_partitioned(containers::array{false}, predicate));
static_assert(containers::is_partitioned(containers::array{true}, predicate));

static_assert(containers::is_partitioned(containers::array{false, false}, predicate));
static_assert(!containers::is_partitioned(containers::array{false, true}, predicate));
static_assert(containers::is_partitioned(containers::array{true, false}, predicate));
static_assert(containers::is_partitioned(containers::array{true, true}, predicate));

static_assert(containers::is_partitioned(containers::array{false, false, false}, predicate));
static_assert(!containers::is_partitioned(containers::array{false, false, true}, predicate));
static_assert(!containers::is_partitioned(containers::array{false, true, false}, predicate));
static_assert(!containers::is_partitioned(containers::array{false, true, true}, predicate));
static_assert(containers::is_partitioned(containers::array{true, false, false}, predicate));
static_assert(!containers::is_partitioned(containers::array{true, false, true}, predicate));
static_assert(containers::is_partitioned(containers::array{true, true, false}, predicate));
static_assert(containers::is_partitioned(containers::array{true, true, true}, predicate));


constexpr bool verify_partition(auto && range) {
	auto const it = containers::partition(range, predicate);
	return containers::is_partitioned(range, predicate) and it == containers::partition_point(range, predicate);
}

static_assert(verify_partition(containers::static_vector<bool, 1>{}));

static_assert(verify_partition(containers::array{false}));
static_assert(verify_partition(containers::array{true}));

static_assert(verify_partition(containers::array{false, false}));
static_assert(verify_partition(containers::array{false, true}));
static_assert(verify_partition(containers::array{true, false}));
static_assert(verify_partition(containers::array{true, true}));

static_assert(verify_partition(containers::array{false, false, false}));
static_assert(verify_partition(containers::array{false, false, true}));
static_assert(verify_partition(containers::array{false, true, false}));
static_assert(verify_partition(containers::array{false, true, true}));
static_assert(verify_partition(containers::array{true, false, false}));
static_assert(verify_partition(containers::array{true, false, true}));
static_assert(verify_partition(containers::array{true, true, false}));
static_assert(verify_partition(containers::array{true, true, true}));

} // namespace