// Copyright David Stone 2021.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <containers/algorithms/ska_sort.hpp>

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

template<typename T, std::size_t size>
constexpr auto test_common_prefix(containers::c_array<T, size> const & source, auto const start_index, std::size_t const expected) {
	auto const prefix = containers::detail::common_prefix(
		containers::range_view(source),
		containers::to_radix_sort_key,
		containers::to_radix_sort_key,
		start_index
	);
	BOUNDED_TEST(prefix == expected);
	return true;
}
static_assert(test_common_prefix<containers::array<std::uint8_t, 1>>(
	{{1}, {1}},
	0_bi,
	1U
));
static_assert(test_common_prefix<containers::array<std::uint8_t, 1>>(
	{{1}, {1}},
	1_bi,
	0U
));


constexpr auto test_sort(auto data, auto function) {
	containers::detail::inplace_radix_sort<1>(containers::range_view(data.input), function);
	BOUNDED_TEST(data.input == data.expected);
	return true;
}

constexpr auto test_sort(auto data) {
	test_sort(std::move(data), containers::to_radix_sort_key);
	return true;
}

constexpr auto test_sort_all(auto range) {
	for (auto & data : range) {
		test_sort(std::move(data));
	}
	return true;
}

constexpr auto test_sort_default_and_copy(auto data) {
	test_sort(data, containers::to_radix_sort_key);
	test_sort(std::move(data), default_copy);
	return true;
}

constexpr auto test_sort_default_and_copy_all(auto range) {
	for (auto & data : range) {
		test_sort(data, containers::to_radix_sort_key);
		test_sort(std::move(data), default_copy);
	}
	return true;
}

static_assert(test_sort(bool_0));
static_assert(test_sort_all(bool_1));
static_assert(test_sort_all(bool_2));
static_assert(test_sort_all(bool_3));
static_assert(test_sort(bool_many));

static_assert(test_sort(uint8_0));
static_assert(test_sort(uint8_1));
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

} // namespace
