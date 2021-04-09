// Copyright David Stone 2021.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <containers/algorithms/double_buffered_ska_sort.hpp>

#include <containers/array/array.hpp>
#include <containers/c_array.hpp>
#include <containers/range_view.hpp>
#include <containers/to_radix_sort_key.hpp>

#include <bounded/integer.hpp>

#include "sort_data.hpp"

#include "../../test_assert.hpp"

namespace {

using namespace bounded::literal;
using namespace containers_test;

template<typename Container>
constexpr auto test_double_buffered_sort(sort_test_data<Container> data, auto function) {
	auto buffer = Container();
	BOUNDED_TEST(containers::size(buffer) == containers::size(data.input));
	bool const data_in_buffer = containers::double_buffered_ska_sort(data.input, buffer, std::move(function));
	auto const & sorted = data_in_buffer ? buffer : data.input;
	BOUNDED_TEST(sorted == data.expected);
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

static_assert(test_double_buffered_sort(uint8_0));
static_assert(test_double_buffered_sort(uint8_1));
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
static_assert(test_double_buffered_sort_default_and_copy_all(array_uint8_1_3));
static_assert(test_double_buffered_sort_default_and_copy(array_uint8_4_many));
static_assert(test_double_buffered_sort_default_and_copy(array_uint16_many));

static_assert(test_double_buffered_sort(tuple_tuple));

static_assert(test_double_buffered_sort(make_move_only_4(), default_copy));
static_assert(test_double_buffered_sort(make_wrapper_5(), get_value_member));

} // namespace
