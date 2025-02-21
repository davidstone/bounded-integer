// Copyright David Stone 2023.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

module;

#include <bounded/assert.hpp>

export module containers.test.ska_sort;

import containers.test.sort.sort_test_data;

import containers.algorithms.sort.inplace_radix_sort;
import containers.algorithms.sort.to_radix_sort_key;

import containers.begin_end;
import containers.subrange;

import std_module;

using namespace containers_test;

constexpr auto test_sort(auto data, auto function) {
	containers::inplace_radix_sort<1, 1>(
		containers::subrange(containers::begin(data.input), containers::end(data.input)),
		function
	);
	return data.input == data.expected;
}

constexpr auto test_sort(auto data) {
	return test_sort(std::move(data), containers::to_radix_sort_key);
}

constexpr auto test_sort_all(auto range) {
	for (auto & data : range) {
		BOUNDED_ASSERT(test_sort(std::move(data)));
	}
	return true;
}

constexpr auto test_sort_default_and_copy(auto data) {
	BOUNDED_ASSERT(test_sort(data, containers::to_radix_sort_key));
	BOUNDED_ASSERT(test_sort(std::move(data), default_copy));
	return true;
}

constexpr auto test_sort_default_and_copy_all(auto range) {
	for (auto & data : range) {
		BOUNDED_ASSERT(test_sort(data, containers::to_radix_sort_key));
		BOUNDED_ASSERT(test_sort(std::move(data), default_copy));
	}
	return true;
}

static_assert(test_sort(bool_0));
static_assert(test_sort_all(bool_1));
static_assert(test_sort_all(bool_2));
static_assert(test_sort_all(bool_3));
static_assert(test_sort(bool_many));

static_assert(test_sort_all(uint8_0));
static_assert(test_sort_all(uint8_1));
static_assert(test_sort_all(uint8_2));
static_assert(test_sort_all(uint8_3));
static_assert(test_sort(uint8_many));
static_assert(test_sort(uint8_256));

static_assert(test_sort(uint16_many));
static_assert(test_sort(uint32_many));
static_assert(test_sort(uint64_many));

static_assert(test_sort(tuple_many));

static_assert(test_sort_default_and_copy(array_uint8_1_1));
static_assert(test_sort_default_and_copy_all(array_uint8_1_2));
static_assert(test_sort_default_and_copy(array_uint8_1_3_one_value));
static_assert(test_sort_default_and_copy_all(array_uint8_1_3_two_values));
static_assert(test_sort_default_and_copy_all(array_uint8_1_3_three_values));
static_assert(test_sort_default_and_copy(array_uint8_4_many));
static_assert(test_sort_default_and_copy(array_uint16_many));

static_assert(test_sort(strings));

static_assert(test_sort(make_vector_int_9()));

static_assert(test_sort(make_vector_vector()));
static_assert(test_sort(make_vector_tuple()));
static_assert(test_sort(make_tuple_vector()));
static_assert(test_sort(tuple_tuple));

static_assert(test_sort(make_move_only(), default_copy));
static_assert(test_sort(make_wrapper(), get_value_member));
