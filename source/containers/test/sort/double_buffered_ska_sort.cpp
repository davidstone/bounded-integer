// Copyright David Stone 2023.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

module;

#include <bounded/assert.hpp>

export module containers.test.double_buffered_ska_sort;

import containers.test.sort.sort_test_data;

import containers.algorithms.sort.double_buffered_ska_sort;
import containers.algorithms.sort.to_radix_sort_key;

import containers.begin_end;
import containers.size;

import std_module;

using namespace containers_test;

template<typename Container>
constexpr auto test_double_buffered_sort(sort_test_data<Container> data, auto function) {
	auto buffer = Container();
	BOUNDED_ASSERT(containers::size(buffer) == containers::size(data.input));
	bool const data_in_buffer = containers::double_buffered_ska_sort(data.input, buffer, std::move(function));
	auto const & sorted = data_in_buffer ? buffer : data.input;
	BOUNDED_ASSERT(sorted == data.expected);
	return true;
}

constexpr auto test_double_buffered_sort(auto data) {
	test_double_buffered_sort(std::move(data), containers::to_radix_sort_key);
	return true;
}

constexpr auto test_double_buffered_sort_all(auto range) {
	for (auto & data : range) {
		test_double_buffered_sort(std::move(data));
	}
	return true;
}

constexpr auto test_double_buffered_sort_default_and_copy(auto data) {
	test_double_buffered_sort(data, containers::to_radix_sort_key);
	test_double_buffered_sort(std::move(data), default_copy);
	return true;
}

constexpr auto test_double_buffered_sort_default_and_copy_all(auto range) {
	for (auto & data : range) {
		test_double_buffered_sort(data, containers::to_radix_sort_key);
		test_double_buffered_sort(std::move(data), default_copy);
	}
	return true;
}

static_assert(test_double_buffered_sort(bool_0));
static_assert(test_double_buffered_sort_all(bool_1));
static_assert(test_double_buffered_sort_all(bool_2));
static_assert(test_double_buffered_sort_all(bool_3));
static_assert(test_double_buffered_sort(bool_many));

static_assert(test_double_buffered_sort_all(uint8_0));
static_assert(test_double_buffered_sort_all(uint8_1));
static_assert(test_double_buffered_sort_all(uint8_2));
static_assert(test_double_buffered_sort_all(uint8_3));
static_assert(test_double_buffered_sort(uint8_many));
static_assert(test_double_buffered_sort(uint8_256));

static_assert(test_double_buffered_sort(uint16_many));
static_assert(test_double_buffered_sort(uint32_many));
static_assert(test_double_buffered_sort(uint64_many));

static_assert(test_double_buffered_sort(tuple_many));

static_assert(test_double_buffered_sort_default_and_copy(array_uint8_1_1));
static_assert(test_double_buffered_sort_default_and_copy_all(array_uint8_1_2));
static_assert(test_double_buffered_sort_default_and_copy(array_uint8_1_3_one_value));
static_assert(test_double_buffered_sort_default_and_copy_all(array_uint8_1_3_two_values));
static_assert(test_double_buffered_sort_default_and_copy_all(array_uint8_1_3_three_values));
static_assert(test_double_buffered_sort_default_and_copy(array_uint8_4_many));
static_assert(test_double_buffered_sort_default_and_copy(array_uint16_many));

static_assert(test_double_buffered_sort(tuple_tuple));

static_assert(test_double_buffered_sort(make_move_only(), default_copy));
static_assert(test_double_buffered_sort(make_wrapper(), get_value_member));
