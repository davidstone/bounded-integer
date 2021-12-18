// Copyright David Stone 2019.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <containers/algorithms/partition.hpp>
#include <containers/array.hpp>
#include <containers/static_vector.hpp>

namespace {

using namespace bounded::literal;

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

constexpr auto test_partition_point(auto const & range, auto const expected_offset) {
	BOUNDED_ASSERT(containers::is_partitioned(range, predicate));
	return containers::partition_point(range, predicate) == containers::begin(range) + expected_offset;
}

static_assert(test_partition_point(
	containers::static_vector<bool, 1>{},
	0_bi
));

static_assert(test_partition_point(
	containers::array{false},
	0_bi
));
static_assert(test_partition_point(
	containers::array{true},
	1_bi
));

static_assert(test_partition_point(
	containers::array{false, false},
	0_bi
));
static_assert(test_partition_point(
	containers::array{true, false},
	1_bi
));
static_assert(test_partition_point(
	containers::array{true, true},
	2_bi
));

static_assert(test_partition_point(
	containers::array{false, false, false},
	0_bi
));
static_assert(test_partition_point(
	containers::array{true, false, false},
	1_bi
));
static_assert(test_partition_point(
	containers::array{true, true, false},
	2_bi
));
static_assert(test_partition_point(
	containers::array{true, true, true},
	3_bi
));

constexpr auto test_partition(auto && input, auto const & expected) -> bool {
	BOUNDED_ASSERT(containers::is_partitioned(expected, predicate));
	auto const it = containers::partition(input, predicate);
	BOUNDED_ASSERT(input == expected);
	BOUNDED_ASSERT(it == containers::partition_point(input, predicate));
	return true;
}

static_assert(test_partition(
	containers::static_vector<bool, 1>{},
	containers::static_vector<bool, 1>{}
));

static_assert(test_partition(
	containers::array{false},
	containers::array{false}
));
static_assert(test_partition(
	containers::array{true},
	containers::array{true}
));

static_assert(test_partition(
	containers::array{false, false},
	containers::array{false, false}
));
static_assert(test_partition(
	containers::array{false, true},
	containers::array{true, false}
));
static_assert(test_partition(
	containers::array{true, false},
	containers::array{true, false}
));
static_assert(test_partition(
	containers::array{true, true},
	containers::array{true, true}
));

static_assert(test_partition(
	containers::array{false, false, false},
	containers::array{false, false, false}
));
static_assert(test_partition(
	containers::array{false, false, true},
	containers::array{true, false, false}
));
static_assert(test_partition(
	containers::array{false, true, false},
	containers::array{true, false, false}
));
static_assert(test_partition(
	containers::array{true, false, false},
	containers::array{true, false, false}
));
static_assert(test_partition(
	containers::array{false, true, true},
	containers::array{true, true, false}
));
static_assert(test_partition(
	containers::array{true, false, true},
	containers::array{true, true, false}
));
static_assert(test_partition(
	containers::array{true, true, false},
	containers::array{true, true, false}
));
static_assert(test_partition(
	containers::array{true, true, true},
	containers::array{true, true, true}
));

} // namespace