// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

module;

#include <bounded/assert.hpp>

export module containers.test.insert;

import containers.algorithms.compare;

import containers.begin_end;
import containers.insert;
import containers.lazy_push_back;
import containers.repeat_n;
import containers.stable_vector;
import containers.vector;

import bounded;
import bounded.test_int;
import std_module;

using namespace bounded::literal;

template<typename Container>
constexpr bool test_range_insert() {
	auto container = Container({1, 2, 3});
	containers::insert(container, containers::begin(container) + 1_bi, containers::repeat_n(3_bi, bounded_test::integer(12)));
	auto const expected = {1, 12, 12, 12, 2, 3};
	return containers::equal(container, expected);
}

static_assert(test_range_insert<containers::stable_vector<bounded_test::integer, 10_bi>>());
static_assert(test_range_insert<containers::vector<bounded_test::integer>>());

template<typename Container>
constexpr bool test_value_insert() {
	auto container = Container({1, 2, 3, 4, 5, 6});
	containers::insert(container, containers::begin(container) + 1_bi, 7);
	auto const expected = {1, 7, 2, 3, 4, 5, 6};
	return containers::equal(container, expected);
}

static_assert(test_value_insert<containers::stable_vector<bounded_test::integer, 10_bi>>());
static_assert(test_value_insert<containers::vector<bounded_test::integer>>());

static_assert(!std::is_trivially_copyable_v<bounded_test::non_copyable_integer>);

constexpr bool test_no_copies() {
	auto container = containers::vector<bounded_test::non_copyable_integer>();
	containers::lazy_push_back(container, bounded::construct<bounded_test::non_copyable_integer>);
	containers::insert(container, begin(container), bounded_test::non_copyable_integer(1));
	return true;
}

static_assert(test_no_copies());