// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <containers/batched.hpp>

#include <containers/algorithms/compare.hpp>
#include <containers/array.hpp>

namespace {

using namespace bounded::literal;

constexpr auto a = containers::array{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

static_assert([] {
	constexpr auto number_of_batches = 1_bi;
	auto const batches = containers::batched(a, number_of_batches);
	BOUNDED_ASSERT(containers::size(batches) == number_of_batches);
	containers::equal(batches[0_bi], a);
	return true;
}());

static_assert([] {
	constexpr auto number_of_batches = 2_bi;
	auto const batches = containers::batched(a, number_of_batches);
	BOUNDED_ASSERT(containers::size(batches) == number_of_batches);
	containers::equal(batches[0_bi], containers::array{0, 1, 2, 3, 4});
	containers::equal(batches[1_bi], containers::array{5, 6, 7, 8, 9});
	return true;
}());

static_assert([] {
	constexpr auto number_of_batches = 3_bi;
	auto const batches = containers::batched(a, number_of_batches);
	BOUNDED_ASSERT(containers::size(batches) == number_of_batches);
	containers::equal(batches[0_bi], containers::array{0, 1, 2, 3});
	containers::equal(batches[1_bi], containers::array{4, 5, 6});
	containers::equal(batches[2_bi], containers::array{7, 8, 9});
	return true;
}());

static_assert([] {
	constexpr auto number_of_batches = 5_bi;
	auto const batches = containers::batched(a, number_of_batches);
	BOUNDED_ASSERT(containers::size(batches) == number_of_batches);
	containers::equal(batches[0_bi], containers::array{0, 1});
	containers::equal(batches[1_bi], containers::array{2, 3});
	containers::equal(batches[2_bi], containers::array{4, 5});
	containers::equal(batches[3_bi], containers::array{6, 7});
	containers::equal(batches[4_bi], containers::array{8, 9});
	return true;
}());

static_assert([] {
	constexpr auto number_of_batches = 6_bi;
	auto const batches = containers::batched(a, number_of_batches);
	BOUNDED_ASSERT(containers::size(batches) == number_of_batches);
	containers::equal(batches[0_bi], containers::array{0, 1});
	containers::equal(batches[1_bi], containers::array{2, 3});
	containers::equal(batches[2_bi], containers::array{4, 5});
	containers::equal(batches[3_bi], containers::array{6, 7});
	containers::equal(batches[4_bi], containers::array{8});
	containers::equal(batches[5_bi], containers::array{9});
	return true;
}());

static_assert([] {
	constexpr auto number_of_batches = 10_bi;
	auto const batches = containers::batched(a, number_of_batches);
	BOUNDED_ASSERT(containers::size(batches) == number_of_batches);
	containers::equal(batches[0_bi], containers::array{0});
	containers::equal(batches[1_bi], containers::array{1});
	containers::equal(batches[2_bi], containers::array{2});
	containers::equal(batches[3_bi], containers::array{3});
	containers::equal(batches[4_bi], containers::array{4});
	containers::equal(batches[5_bi], containers::array{5});
	containers::equal(batches[6_bi], containers::array{6});
	containers::equal(batches[7_bi], containers::array{7});
	containers::equal(batches[8_bi], containers::array{8});
	containers::equal(batches[9_bi], containers::array{9});
	return true;
}());

static_assert([] {
	constexpr auto number_of_batches = 12_bi;
	auto const batches = containers::batched(a, number_of_batches);
	BOUNDED_ASSERT(containers::size(batches) == number_of_batches);
	containers::equal(batches[0_bi], containers::array{0});
	containers::equal(batches[1_bi], containers::array{1});
	containers::equal(batches[2_bi], containers::array{2});
	containers::equal(batches[3_bi], containers::array{3});
	containers::equal(batches[4_bi], containers::array{4});
	containers::equal(batches[5_bi], containers::array{5});
	containers::equal(batches[6_bi], containers::array{6});
	containers::equal(batches[7_bi], containers::array{7});
	containers::equal(batches[8_bi], containers::array{8});
	containers::equal(batches[9_bi], containers::array{9});
	containers::equal(batches[10_bi], containers::array<int, 0>{});
	containers::equal(batches[11_bi], containers::array<int, 0>{});
	return true;
}());

static_assert([] {
	constexpr auto empty = containers::array<int, 0>();
	constexpr auto number_of_batches = 1_bi;
	auto const batches = containers::batched(empty, number_of_batches);
	BOUNDED_ASSERT(containers::size(batches) == number_of_batches);
	containers::equal(batches[0_bi], empty);
	return true;
}());

} // namespace