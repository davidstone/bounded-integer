// Copyright David Stone 2019, Malte Skarupke 2016.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <containers/algorithms/ska_sort.hpp>
#include <containers/algorithms/sort.hpp>
#include <containers/array/array.hpp>
#include <containers/array/make_array.hpp>
#include <containers/reserve_if_reservable.hpp>
#include <containers/static_vector/static_vector.hpp>
#include <containers/vector.hpp>

#include <bounded/detail/tuple.hpp>
#include <bounded/copy.hpp>

#include <gtest/gtest.h>
#include <benchmark/benchmark.h>

#include <deque>
#include <random>
#include <vector>

#include "../../test_assert.hpp"

using namespace containers;

template<typename T, std::size_t size>
using c_array = T[size];

template<typename T, std::size_t size, std::size_t... indexes>
constexpr auto copy_from_c_array(c_array<T, size> const & original, std::index_sequence<indexes...>) {
	return containers::array<std::remove_cvref_t<T>, sizeof...(indexes)>{original[indexes]...};
}

static constexpr void inplace_radix_sort(auto begin, auto end, auto && extract_key) {
	detail::inplace_radix_sort<1>(begin, end, extract_key);
}

static constexpr void inplace_radix_sort(auto begin, auto end) {
	inplace_radix_sort(begin, end, bounded::identity);
}

template<typename Container>
constexpr bool test_sort_copy(Container source, Container const & expected, auto function) {
	BOUNDED_TEST(containers::is_sorted(expected));
	auto other = Container();
	if constexpr (requires(Container c) { c.resize(size(c)); }) {
		other.resize(size(source));
	}
	bool const data_in_other = ska_sort_copy(source, other, std::move(function));
	auto const & sorted = data_in_other ? other : source;
	BOUNDED_TEST(sorted == expected);
	return true;
}

constexpr bool test_sort_inplace(auto source, auto const & expected, auto function) {
	BOUNDED_TEST(containers::is_sorted(expected));
	inplace_radix_sort(begin(source), end(source), std::move(function));
	BOUNDED_TEST(source == expected);
	return true;
}

constexpr bool test_sort(auto original, auto const & expected, auto function) {
	test_sort_copy(original, expected, function);
	test_sort_inplace(std::move(original), expected, std::move(function));
	return true;
}

constexpr bool test_sort(auto original, auto const & expected) {
	test_sort(original, expected, bounded::identity);
	test_sort(std::move(original), expected, bounded::copy);
	return true;
}


template<typename T, std::size_t size>
constexpr bool test_sort(c_array<T, size> const & original, c_array<T, size> const & expected, auto function) {
	return test_sort(
		copy_from_c_array(original, std::make_index_sequence<size>{}),
		copy_from_c_array(expected, std::make_index_sequence<size>{}),
		std::move(function)
	);
}

template<typename T, std::size_t size>
constexpr bool test_sort(c_array<T, size> const & original, c_array<T, size> const & expected) {
	test_sort(original, expected, bounded::identity);
	test_sort(original, expected, bounded::copy);
	return true;
}

template<typename T, std::size_t size>
constexpr bool test_common_prefix(c_array<T, size> const & source, std::size_t const start_index, std::size_t const expected) {
	auto const prefix = containers::detail::common_prefix(
		std::begin(source),
		std::end(source),
		start_index,
		bounded::identity,
		bounded::identity
	);
	BOUNDED_TEST(prefix == expected);
	return true;
}
static_assert(test_common_prefix<std::array<std::uint8_t, 1>>(
	{{1}, {1}},
	0U,
	1U
));
static_assert(test_common_prefix<std::array<std::uint8_t, 1>>(
	{{1}, {1}},
	1U,
	0U
));

static_assert(test_sort(
	containers::static_vector<bool, 1>{},
	containers::static_vector<bool, 1>{}
));

static_assert(test_sort<bool>(
	{false},
	{false}
));

static_assert(test_sort<bool>(
	{true},
	{true}
));

static_assert(test_sort<bool>(
	{false, false},
	{false, false}
));

static_assert(test_sort<bool>(
	{false, true},
	{false, true}
));

static_assert(test_sort<bool>(
	{true, false},
	{false, true}
));

static_assert(test_sort<bool>(
	{true, true},
	{true, true}
));

static_assert(test_sort<bool>(
	{false, false, false},
	{false, false, false}
));

static_assert(test_sort<bool>(
	{false, false, true},
	{false, false, true}
));

static_assert(test_sort<bool>(
	{false, true, false},
	{false, false, true}
));

static_assert(test_sort<bool>(
	{true, false, false},
	{false, false, true}
));

static_assert(test_sort<bool>(
	{false, true, true},
	{false, true, true}
));

static_assert(test_sort<bool>(
	{true, false, true},
	{false, true, true}
));

static_assert(test_sort<bool>(
	{true, true, false},
	{false, true, true}
));

static_assert(test_sort<bool>(
	{true, true, true},
	{true, true, true}
));

static_assert(test_sort<bool>(
	{true, false, true, false, true, false, false, true, true, false},
	{false, false, false, false, false, true, true, true, true, true}
));


static_assert(test_sort(
	containers::static_vector<std::uint8_t, 1>{},
	containers::static_vector<std::uint8_t, 1>{}
));
static_assert(test_sort<std::uint8_t>(
	{1},
	{1}
));

static_assert(test_sort<std::uint8_t>(
	{1, 1},
	{1, 1}
));
static_assert(test_sort<std::uint8_t>(
	{1, 2},
	{1, 2}
));
static_assert(test_sort<std::uint8_t>(
	{2, 1},
	{1, 2}
));

static_assert(test_sort<std::uint8_t>(
	{1, 1, 1},
	{1, 1, 1}
));
static_assert(test_sort<std::uint8_t>(
	{1, 1, 2},
	{1, 1, 2}
));
static_assert(test_sort<std::uint8_t>(
	{1, 2, 1},
	{1, 1, 2}
));
static_assert(test_sort<std::uint8_t>(
	{2, 1, 1},
	{1, 1, 2}
));
static_assert(test_sort<std::uint8_t>(
	{1, 2, 2},
	{1, 2, 2}
));
static_assert(test_sort<std::uint8_t>(
	{2, 1, 2},
	{1, 2, 2}
));
static_assert(test_sort<std::uint8_t>(
	{2, 2, 1},
	{1, 2, 2}
));
static_assert(test_sort<std::uint8_t>(
	{1, 2, 3},
	{1, 2, 3}
));
static_assert(test_sort<std::uint8_t>(
	{1, 3, 2},
	{1, 2, 3}
));
static_assert(test_sort<std::uint8_t>(
	{2, 1, 3},
	{1, 2, 3}
));
static_assert(test_sort<std::uint8_t>(
	{2, 3, 1},
	{1, 2, 3}
));
static_assert(test_sort<std::uint8_t>(
	{3, 1, 2},
	{1, 2, 3}
));
static_assert(test_sort<std::uint8_t>(
	{3, 2, 1},
	{1, 2, 3}
));

static_assert(test_sort<std::uint8_t>(
	{5, 6, 19, 2, 5, 0, 7, 23, 6, 8, 99},
	{0, 2, 5, 5, 6, 6, 7, 8, 19, 23, 99}
));

constexpr auto uint8_256_items = []{
	auto to_sort = containers::make_array_n(256_bi, static_cast<std::uint8_t>(254));
	back(to_sort) = 255;
	return to_sort;
}();
static_assert(test_sort(
	uint8_256_items,
	uint8_256_items
));

static_assert(test_sort<std::int8_t>(
	{5, 6, 19, -4, 2, 5, 0, -55, 7, 23, 6, 8, 127, -128, 99},
	{-128, -55, -4, 0, 2, 5, 5, 6, 6, 7, 8, 19, 23, 99, 127}
));

static_assert(test_sort<std::uint16_t>(
	{5, 6, 19, 2, 5, 7, 0, 23, 6, 256, 255, 8, 99, 1024, 65535, 65534},
	{0, 2, 5, 5, 6, 6, 7, 8, 19, 23, 99, 255, 256, 1024, 65534, 65535}
));

static_assert(test_sort<std::int16_t>(
	{5, 6, 19, -4, 2, 5, 0, -55, 7, 1000, 23, 6, 8, 127, -128, -129, -256, -32768, 32767, 99},
	{-32768, -256, -129, -128, -55, -4, 0, 2, 5, 5, 6, 6, 7, 8, 19, 23, 99, 127, 1000, 32767}
));

static_assert(test_sort<std::uint32_t>(
	{
		5,
		6,
		19,
		2,
		5,
		7,
		0,
		23,
		6,
		256,
		255,
		8,
		99,
		1024,
		65536,
		65535,
		65534,
		1000000,
		bounded::max_value<std::uint32_t>
	},
	{
		0,
		2,
		5,
		5,
		6,
		6,
		7,
		8,
		19,
		23,
		99,
		255,
		256,
		1024,
		65534,
		65535,
		65536,
		1000000,
		bounded::max_value<std::uint32_t>
	}
));

static_assert(test_sort<std::int32_t>(
	{
		5,
		6,
		19,
		-4,
		2,
		5,
		0,
		-55,
		7,
		1000,
		23,
		6,
		8,
		127,
		-128,
		-129,
		-256,
		32768,
		-32769,
		-32768,
		32767,
		99,
		1000000,
		-1000001,
		bounded::min_value<std::int32_t>,
		bounded::max_value<std::int32_t>,
		bounded::max_value<std::int32_t> - 1,
		bounded::min_value<std::int32_t> + 1
	},
	{
		bounded::min_value<std::int32_t>,
		bounded::min_value<std::int32_t> + 1,
		-1000001,
		-32769,
		-32768,
		-256,
		-129,
		-128,
		-55,
		-4,
		0,
		2,
		5,
		5,
		6,
		6,
		7,
		8,
		19,
		23,
		99,
		127,
		1000,
		32767,
		32768,
		1000000,
		bounded::max_value<std::int32_t> - 1,
		bounded::max_value<std::int32_t>
	}
));

static_assert(test_sort<std::uint64_t>(
	{
		5,
		6,
		19,
		2,
		5,
		7,
		0,
		static_cast<std::uint64_t>(bounded::max_value<std::uint32_t>) + 1,
		1'000'000'000'000,
		bounded::max_value<std::uint64_t>,
		23,
		6,
		256,
		255,
		8,
		99,
		1'024,
		65'536,
		65'535,
		65'534,
		1'000'000,
		bounded::max_value<std::uint32_t>,
	},
	{
		0,
		2,
		5,
		5,
		6,
		6,
		7,
		8,
		19,
		23,
		99,
		255,
		256,
		1'024,
		65'534,
		65'535,
		65'536,
		1'000'000,
		bounded::max_value<std::uint32_t>,
		static_cast<std::uint64_t>(bounded::max_value<std::uint32_t>) + 1,
		1'000'000'000'000,
		bounded::max_value<std::uint64_t>,
	}
));

static_assert(test_sort<std::int64_t>(
	{
		5,
		6,
		19,
		static_cast<std::int64_t>(bounded::min_value<std::int32_t>) - 1,
		bounded::min_value<std::int64_t>,
		-1'000'000'000'000,
		1'000'000'000'000,
		bounded::max_value<std::int32_t>,
		bounded::max_value<std::int64_t>,
		-4,
		2,
		5,
		0,
		-55,
		7,
		1'000,
		23,
		6,
		8,
		127,
		-128,
		-129,
		-256,
		32'768,
		-32'769,
		-32'768,
		32'767,
		99,
		1'000'000,
		-1'000'001,
		bounded::min_value<std::int32_t>,
		bounded::max_value<std::int32_t>,
		bounded::max_value<std::int32_t> - 1,
		bounded::min_value<std::int32_t> + 1,
	},
	{
		bounded::min_value<int64_t>,
		-1'000'000'000'000,
		static_cast<std::int64_t>(bounded::min_value<std::int32_t>) - 1,
		bounded::min_value<std::int32_t>,
		bounded::min_value<std::int32_t> + 1,
		-1'000'001,
		-32'769,
		-32'768,
		-256,
		-129,
		-128,
		-55,
		-4,
		0,
		2,
		5,
		5,
		6,
		6,
		7,
		8,
		19,
		23,
		99,
		127,
		1'000,
		32'767,
		32'768,
		1'000'000,
		bounded::max_value<std::int32_t> - 1,
		bounded::max_value<std::int32_t>,
		bounded::max_value<std::int32_t>,
		1'000'000'000'000,
		bounded::max_value<std::int64_t>,
	}
));

TEST(radix_sort, float) {
	test_sort<float>(
		{
			5,
			6,
			19,
			std::numeric_limits<float>::infinity(),
			-std::numeric_limits<float>::infinity(),
			-4,
			2,
			5,
			0,
			-55,
			7,
			1000,
			23,
			6,
			8,
			127,
			-128,
			-129,
			-256,
			32768,
			-32769,
			-32768,
			32767,
			99,
			1000000,
			-1000001,
			0.1F,
			2.5F,
			17.8F,
			-12.4F,
			-0.0000002F,
			-0.0F,
			-777777777.7F,
			444444444444.4F,
			std::nextafter(0.0F, 1.0F),
			std::nextafter(std::nextafter(0.0F, 1.0F), 1.0F),
			std::nextafter(-0.0F, -1.0F),
			std::nextafter(std::nextafter(-0.0F, -1.0F), -1.0F),
		},
		{
			-std::numeric_limits<float>::infinity(),
			-777777777.7f,
			-1000001,
			-32769,
			-32768,
			-256,
			-129,
			-128,
			-55,
			-12.4f,
			-4,
			-0.0000002f,
			std::nextafter(std::nextafter(-0.0F, -1.0F), -1.0F),
			std::nextafter(-0.0F, -1.0F),
			-0.0f,
			0,
			std::nextafter(0.0F, 1.0F),
			std::nextafter(std::nextafter(0.0F, 1.0F), 1.0F),
			0.1f,
			2,
			2.5f,
			5,
			5,
			6,
			6,
			7,
			8,
			17.8f,
			19,
			23,
			99,
			127,
			1000,
			32767,
			32768,
			1000000,
			444444444444.4f,
			std::numeric_limits<float>::infinity(),
		}
	);
}
TEST(radix_sort, double) {
	test_sort<double>(
		{
			5,
			6,
			19,
			std::numeric_limits<double>::infinity(),
			-std::numeric_limits<double>::infinity(),
			-4,
			2,
			5,
			0,
			-55,
			7,
			1000,
			23,
			6,
			8,
			127,
			-128,
			-129,
			-256,
			32768,
			-32769,
			-32768,
			32767,
			99,
			1000000,
			-1000001,
			0.1,
			2.5,
			17.8,
			-12.4,
			-0.0000002,
			-0.0,
			-777777777.7,
			444444444444.4,
			std::nextafter(0.0, 1.0),
			std::nextafter(std::nextafter(0.0, 1.0), 1.0),
			std::nextafter(-0.0, -1.0),
			std::nextafter(std::nextafter(-0.0, -1.0), -1.0),
		},
		{
			-std::numeric_limits<double>::infinity(),
			-777777777.7,
			-1000001,
			-32769,
			-32768,
			-256,
			-129,
			-128,
			-55,
			-12.4,
			-4,
			-0.0000002,
			std::nextafter(std::nextafter(-0.0, -1.0), -1.0),
			std::nextafter(-0.0, -1.0),
			-0.0,
			0,
			std::nextafter(0.0, 1.0),
			std::nextafter(std::nextafter(0.0, 1.0), 1.0),
			0.1,
			2,
			2.5,
			5,
			5,
			6,
			6,
			7,
			8,
			17.8,
			19,
			23,
			99,
			127,
			1000,
			32767,
			32768,
			1000000,
			444444444444.4,
			std::numeric_limits<double>::infinity(),
		}
	);
}

namespace {

template<typename Char, std::size_t size>
constexpr auto array_from_c_string(c_array<Char, size> const & str) {
	return copy_from_c_array(str, std::make_index_sequence<size - 1U>{});
}

} // namespace

// TODO: This assumes a particular sort order based on ASCII; maybe use
// std::sort for the baseline?
static_assert(test_sort(
	array_from_c_string("Hello, World!"),
	array_from_c_string(" !,HWdellloor")
));
static_assert(test_sort(
	array_from_c_string(u"Hello, World!"),
	array_from_c_string(u" !,HWdellloor")
));
static_assert(test_sort(
	array_from_c_string(U"Hello, World!"),
	array_from_c_string(U" !,HWdellloor")
));
static_assert(test_sort(
	array_from_c_string(L"Hello, World!"),
	array_from_c_string(L" !,HWdellloor")
));

static_assert(test_sort<bounded::tuple<int, bool>>(
	{{5, true}, {5, false}, {6, false}, {7, true}, {4, false}, {4, true}},
	{{4, false}, {4, true}, {5, false}, {5, true}, {6, false}, {7, true}}
));

static_assert(test_sort<bounded::tuple<bool, int>>(
	{{true, 5}, {false, 5}, {false, 6}, {true, 7}, {false, 4}, {true, 4}},
	{{false, 4}, {false, 5}, {false, 6}, {true, 4}, {true, 5}, {true, 7}}
));

static_assert(test_sort<bounded::tuple<bool, int, bool>>(
	{
		{true, 5, true},
		{true, 5, false},
		{false, 6, false},
		{true, 7, true},
		{true, 4, false},
		{false, 4, true},
		{false, 5, false},
	},
	{
		{false, 4, true},
		{false, 5, false},
		{false, 6, false},
		{true, 4, false},
		{true, 5, false},
		{true, 5, true},
		{true, 7, true},
	}
));

static_assert(test_sort<std::array<std::uint8_t, 1>>(
	{{1}},
	{{1}}
));

static_assert(test_sort<std::array<std::uint8_t, 1>>(
	{{1}, {1}},
	{{1}, {1}}
));

static_assert(test_sort<std::array<std::uint8_t, 1>>(
	{{1}, {2}},
	{{1}, {2}}
));

static_assert(test_sort<std::array<std::uint8_t, 1>>(
	{{2}, {1}},
	{{1}, {2}}
));

static_assert(test_sort<std::array<std::uint8_t, 1>>(
	{{2}, {2}},
	{{2}, {2}}
));

static_assert(test_sort<std::array<std::uint8_t, 1>>(
	{{1}, {1}, {1}},
	{{1}, {1}, {1}}
));

static_assert(test_sort<std::array<std::uint8_t, 1>>(
	{{1}, {1}, {2}},
	{{1}, {1}, {2}}
));

static_assert(test_sort<std::array<std::uint8_t, 1>>(
	{{1}, {2}, {1}},
	{{1}, {1}, {2}}
));

static_assert(test_sort<std::array<std::uint8_t, 1>>(
	{{2}, {1}, {1}},
	{{1}, {1}, {2}}
));

static_assert(test_sort<std::array<std::uint8_t, 1>>(
	{{1}, {2}, {2}},
	{{1}, {2}, {2}}
));

static_assert(test_sort<std::array<std::uint8_t, 1>>(
	{{2}, {1}, {2}},
	{{1}, {2}, {2}}
));

static_assert(test_sort<std::array<std::uint8_t, 1>>(
	{{2}, {2}, {1}},
	{{1}, {2}, {2}}
));

static_assert(test_sort<std::array<std::uint8_t, 1>>(
	{{2}, {2}, {2}},
	{{2}, {2}, {2}}
));

static_assert(test_sort<std::array<std::uint8_t, 4>>(
	{
		{1, 2, 3, 4},
		{0, 3, 4, 5},
		{1, 1, 2, 2},
		{1, 2, 2, 4},
		{1, 2, 2, 3},
		{0, 3, 4, 4},
	},
	{
		{0, 3, 4, 4},
		{0, 3, 4, 5},
		{1, 1, 2, 2},
		{1, 2, 2, 3},
		{1, 2, 2, 4},
		{1, 2, 3, 4},
	}
));

static_assert(test_sort<std::array<int, 4>>(
	{
		{1, 2, 3, 4},
		{0, 3, 4, 5},
		{1, 1, 2, 2},
		{1, 2, 2, 4},
		{1, 2, 2, 3},
		{0, 3, 4, 4},
	},
	{
		{0, 3, 4, 4},
		{0, 3, 4, 5},
		{1, 1, 2, 2},
		{1, 2, 2, 3},
		{1, 2, 2, 4},
		{1, 2, 3, 4},
	}
));

struct move_only {
	move_only() = default;

	constexpr move_only(int x):
		value(x)
	{
	}

	constexpr move_only(move_only && other) noexcept:
		value(other.value),
		moved_from(other.moved_from)
	{
		other.moved_from = true;
	}

	constexpr move_only & operator=(move_only && other) & noexcept {
		value = other.value;
		moved_from = other.moved_from;
		other.moved_from = true;
		return *this;
	}

	friend constexpr auto operator==(move_only const & lhs, move_only const & rhs) -> bool {
		BOUNDED_TEST(!lhs.moved_from and !rhs.moved_from);
		return lhs.value == rhs.value;
	}
	friend constexpr auto operator<=>(move_only const & lhs, move_only const & rhs) {
		BOUNDED_TEST(!lhs.moved_from and !rhs.moved_from);
		return lhs.value <=> rhs.value;
	}

	constexpr auto get() const {
		BOUNDED_TEST(!moved_from);
		return value;
	}

private:
	int value = 0;
	bool moved_from = false;
};

BOUNDED_COMPARISON

static_assert(test_sort_copy(
	containers::array<move_only, 4>{
		5,
		0,
		1234567,
		-1000,
	},
	containers::array<move_only, 4>{
		-1000,
		0,
		5,
		1234567,
	},
	[](auto const & value) { return value.get(); }
));

static_assert(test_sort_inplace(
	containers::array<move_only, 4>{
		5,
		0,
		1234567,
		-1000,
	},
	containers::array<move_only, 4>{
		-1000,
		0,
		5,
		1234567,
	},
	[](auto const & value) { return value.get(); }
));

TEST(radix_sort, vector_bool) {
	test_sort(
		std::vector<bool>{true, false, true, true, false, true, true, true, false, true, false, false},
		std::vector<bool>{false, false, false, false, false, true, true, true, true, true, true, true}
	);
}


// These two were labelled as "error cases" in the original unit tests
static_assert(test_sort<std::int8_t>(
	{46, 7, 33, -78, -114, -78, 33, 82},
	{-114, -78, -78, 7, 33, 33, 46, 82}
));

static_assert(test_sort<std::int8_t>(
	{-104, 50, 108, 105, 112, 53, 47, 102},
	{-104, 47, 50, 53, 102, 105, 108, 112}
));

static_assert(test_sort<bounded::tuple<bool, bounded::tuple<int, bounded::tuple<int, int>>, bounded::tuple<bool, bool, bool>>>(
	{
		{true, bounded::tuple(5, bounded::tuple(6, 7)), bounded::tuple(true, false, true)},
		{false, bounded::tuple(5, bounded::tuple(6, 7)), bounded::tuple(true, false, true)},
		{false, bounded::tuple(5, bounded::tuple(6, 8)), bounded::tuple(true, false, true)},
		{false, bounded::tuple(5, bounded::tuple(6, 6)), bounded::tuple(true, false, true)},
		{false, bounded::tuple(5, bounded::tuple(7, 6)), bounded::tuple(true, false, true)},
		{false, bounded::tuple(5, bounded::tuple(7, 6)), bounded::tuple(true, true, true)},
		{false, bounded::tuple(5, bounded::tuple(7, 6)), bounded::tuple(true, true, false)},
		{false, bounded::tuple(5, bounded::tuple(7, 6)), bounded::tuple(false, true, false)},
		{false, bounded::tuple(4, bounded::tuple(7, 6)), bounded::tuple(false, true, false)},
	},
	{
		{false, bounded::tuple(4, bounded::tuple(7, 6)), bounded::tuple(false, true, false)},
		{false, bounded::tuple(5, bounded::tuple(6, 6)), bounded::tuple(true, false, true)},
		{false, bounded::tuple(5, bounded::tuple(6, 7)), bounded::tuple(true, false, true)},
		{false, bounded::tuple(5, bounded::tuple(6, 8)), bounded::tuple(true, false, true)},
		{false, bounded::tuple(5, bounded::tuple(7, 6)), bounded::tuple(false, true, false)},
		{false, bounded::tuple(5, bounded::tuple(7, 6)), bounded::tuple(true, false, true)},
		{false, bounded::tuple(5, bounded::tuple(7, 6)), bounded::tuple(true, true, false)},
		{false, bounded::tuple(5, bounded::tuple(7, 6)), bounded::tuple(true, true, true)},
		{true, bounded::tuple(5, bounded::tuple(6, 7)), bounded::tuple(true, false, true)},
	}
));

static_assert(containers::range<std::string>);

static_assert(test_sort_inplace(
	containers::array<std::string_view, 8>{
		"Hi",
		"There",
		"Hello",
		"World!",
		"Foo",
		"Bar",
		"Baz",
		"",
	},
	containers::array<std::string_view, 8>{
		"",
		"Bar",
		"Baz",
		"Foo",
		"Hello",
		"Hi",
		"There",
		"World!",
	},
	bounded::identity
));
static_assert(test_sort_inplace(
	containers::array<std::string_view, 8>{
		"Hi",
		"There",
		"Hello",
		"World!",
		"Foo",
		"Bar",
		"Baz",
		"",
	},
	containers::array<std::string_view, 8>{
		"",
		"Bar",
		"Baz",
		"Foo",
		"Hello",
		"Hi",
		"There",
		"World!",
	},
	bounded::copy
));

TEST(radix_sort, vector) {
	test_sort_inplace(
		std::vector<std::vector<int>>{
			{ 1, 2, 3 },
			{ 1, 2, 2 },
			{ 1, 3, 2 },
			{ 2, 3, 2 },
			{ 2, 3, 2, 4 },
			{ 2, 3, 2, 4, 5 },
			{ 3, 2, 4, 5 },
			{ 1 },
			{},
		},
		std::vector<std::vector<int>>{
			{},
			{ 1 },
			{ 1, 2, 2 },
			{ 1, 2, 3 },
			{ 1, 3, 2 },
			{ 2, 3, 2 },
			{ 2, 3, 2, 4 },
			{ 2, 3, 2, 4, 5 },
			{ 3, 2, 4, 5 },
		},
		bounded::identity
	);
	test_sort_inplace(
		std::vector<std::vector<int>>{
			{ 1, 2, 3 },
			{ 1, 2, 2 },
			{ 1, 3, 2 },
			{ 2, 3, 2 },
			{ 2, 3, 2, 4 },
			{ 2, 3, 2, 4, 5 },
			{ 3, 2, 4, 5 },
			{ 1 },
			{},
		},
		std::vector<std::vector<int>>{
			{},
			{ 1 },
			{ 1, 2, 2 },
			{ 1, 2, 3 },
			{ 1, 3, 2 },
			{ 2, 3, 2 },
			{ 2, 3, 2, 4 },
			{ 2, 3, 2, 4, 5 },
			{ 3, 2, 4, 5 },
		},
		bounded::copy
	);
}

TEST(radix_sort, string_in_vector) {
	test_sort_inplace(
		std::vector<std::vector<std::string>>{
			{ "hi", "there", "you" },
			{ "are", "probably", "not", "going" },
			{ "to", "pass" },
			{ "" },
			{ },
			{ "this", "test", "the", "first" },
			{ "time" },
			{ "oh it did pass", "n", "e", "a", "t!" },
			{ "hi", "there", "I", "added", "more", "tests" },
			{ "hi", "there", "needed", "the", "same", "prefix" },
		},
		std::vector<std::vector<std::string>>{
			{ },
			{ "" },
			{ "are", "probably", "not", "going" },
			{ "hi", "there", "I", "added", "more", "tests" },
			{ "hi", "there", "needed", "the", "same", "prefix" },
			{ "hi", "there", "you" },
			{ "oh it did pass", "n", "e", "a", "t!" },
			{ "this", "test", "the", "first" },
			{ "time" },
			{ "to", "pass" },
		},
		bounded::identity
	);
	#if 0
	test_sort_inplace(
		std::vector<std::vector<std::string>>{
			{ "hi", "there", "you" },
			{ "are", "probably", "not", "going" },
			{ "to", "pass" },
			{ "" },
			{ },
			{ "this", "test", "the", "first" },
			{ "time" },
			{ "oh it did pass", "n", "e", "a", "t!" },
			{ "hi", "there", "I", "added", "more", "tests" },
			{ "hi", "there", "needed", "the", "same", "prefix" },
		},
		std::vector<std::vector<std::string>>{
			{ },
			{ "" },
			{ "are", "probably", "not", "going" },
			{ "hi", "there", "I", "added", "more", "tests" },
			{ "hi", "there", "needed", "the", "same", "prefix" },
			{ "hi", "there", "you" },
			{ "oh it did pass", "n", "e", "a", "t!" },
			{ "this", "test", "the", "first" },
			{ "time" },
			{ "to", "pass" },
		},
		identity_by_value
	);
	#endif
}

TEST(radix_sort, vector_tuple_string_reference) {
	test_sort_inplace(
		std::vector<std::tuple<std::string, std::string>>{
			{"hi", "there"},
			{"you", "are"},
			{"probably", "not"},
			{"going", "to"},
			{"pass", ""},
			{"", ""},
			{"", "this"},
			{"test", "the"},
			{"first", "time"},
			{"oh it did pass", "n"},
			{"e", "a"},
			{"t!", ""},
			{"hi", "there"},
			{"I", "added"},
			{"more", "tests"},
			{"hi", "there"},
			{"needed", "the"},
			{"same", "prefix"},
		},
		std::vector<std::tuple<std::string, std::string>>{
			{"", ""},
			{"", "this"},
			{"I", "added"},
			{"e", "a"},
			{"first", "time"},
			{"going", "to"},
			{"hi", "there"},
			{"hi", "there"},
			{"hi", "there"},
			{"more", "tests"},
			{"needed", "the"},
			{"oh it did pass", "n"},
			{"pass", ""},
			{"probably", "not"},
			{"same", "prefix"},
			{"t!", ""},
			{"test", "the"},
			{"you", "are"},
		},
		bounded::identity
	);
}

#if 0
TEST(radix_sort, vector_tuple_string_value) {
	test_sort_inplace(
		std::vector<std::tuple<std::string, std::string>>{
			{"hi", "there"},
			{"you", "are"},
			{"probably", "not"},
			{"going", "to"},
			{"pass", ""},
			{"", ""},
			{"", "this"},
			{"test", "the"},
			{"first", "time"},
			{"oh it did pass", "n"},
			{"e", "a"},
			{"t!", ""},
			{"hi", "there"},
			{"I", "added"},
			{"more", "tests"},
			{"hi", "there"},
			{"needed", "the"},
			{"same", "prefix"},
		},
		std::vector<std::tuple<std::string, std::string>>{
			{"", ""},
			{"", "this"},
			{"I", "added"},
			{"e", "a"},
			{"first", "time"},
			{"going", "to"},
			{"hi", "there"},
			{"hi", "there"},
			{"hi", "there"},
			{"more", "tests"},
			{"needed", "the"},
			{"oh it did pass", "n"},
			{"pass", ""},
			{"probably", "not"},
			{"same", "prefix"},
			{"t!", ""},
			{"test", "the"},
			{"you", "are"},
		},
		identity_by_value
	);
}
#endif

TEST(radix_sort, pointers) {
	constexpr int array[] = { 1, 2, 3, 4, 5, 6, 7, 8 };
	test_sort<int const *>(
		{
			array + 1,
			array + 3,
			array,
			array + 2,
			array + 7,
			array + 8,
			array + 6,
			array + 4,
			array + 5,
		},
		{
			array,
			array + 1,
			array + 2,
			array + 3,
			array + 4,
			array + 5,
			array + 6,
			array + 7,
			array + 8,
		}
	);
}

template<bool by_value>
struct move_only_with_to_sort_key {
	move_only_with_to_sort_key() = default;

	constexpr move_only_with_to_sort_key(int x):
		value(x)
	{
	}

	constexpr move_only_with_to_sort_key(move_only_with_to_sort_key && other) noexcept:
		value(other.value),
		moved_from(other.moved_from)
	{
		other.moved_from = true;
	}

	constexpr move_only_with_to_sort_key & operator=(move_only_with_to_sort_key && other) & noexcept {
		value = other.value;
		moved_from = other.moved_from;
		other.moved_from = true;
		return *this;
	}

	friend constexpr auto operator==(move_only_with_to_sort_key const & lhs, move_only_with_to_sort_key const & rhs) -> bool {
		BOUNDED_TEST(!lhs.moved_from and !rhs.moved_from);
		return lhs.value == rhs.value;
	}
	friend constexpr auto operator<=>(move_only_with_to_sort_key const & lhs, move_only_with_to_sort_key const & rhs) {
		BOUNDED_TEST(!lhs.moved_from and !rhs.moved_from);
		return lhs.value <=> rhs.value;
	}

	friend constexpr auto to_radix_sort_key(move_only_with_to_sort_key const & arg) -> std::conditional_t<by_value, int, int const &> {
		BOUNDED_TEST(!arg.moved_from);
		return arg.value;
	}

private:
	int value = 0;
	bool moved_from = false;
};

template<bool by_value>
struct wrapper {
	move_only_with_to_sort_key<by_value> value;
	friend constexpr auto operator==(wrapper const & lhs, wrapper const & rhs) -> bool {
		return to_radix_sort_key(lhs.value) == to_radix_sort_key(rhs.value);
	}
	friend constexpr auto operator<=>(wrapper const & lhs, wrapper const & rhs) {
		return to_radix_sort_key(lhs.value) <=> to_radix_sort_key(rhs.value);
	}
};

constexpr auto get_value_member = [](auto const & w) { return bounded::tie(w.value); };
static_assert(test_sort_copy(
	containers::array<wrapper<true>, 5>{
		wrapper<true>{2},
		wrapper<true>{3},
		wrapper<true>{1},
		wrapper<true>{5},
		wrapper<true>{4},
	},
	containers::array<wrapper<true>, 5>{
		wrapper<true>{1},
		wrapper<true>{2},
		wrapper<true>{3},
		wrapper<true>{4},
		wrapper<true>{5},
	},
	get_value_member
));

// benchmarks

#define SKA_SORT_NOINLINE __attribute__((noinline))

template<typename Container>
static auto create_radix_sort_data(std::mt19937_64 & engine, std::int64_t const size, auto distribution) {
	auto result = Container();
	containers::detail::reserve_if_reservable(result, static_cast<typename Container::size_type>(size));
	for (std::int64_t n = 0; n != size; ++n) {
		containers::push_back(result, distribution(engine));
	}
	return result;
}

static auto create_radix_sort_data(std::mt19937_64 & engine, std::int64_t const size, auto distribution) {
	return create_radix_sort_data<std::vector<decltype(distribution(engine))>>(engine, size, distribution);
}


#if 0

extern const std::vector<const char *> & get_word_list();
static std::vector<std::string> SKA_SORT_NOINLINE create_radix_sort_data(std::mt19937_64 & randomness, std::int64_t size)
{
	const std::vector<const char *> & words = get_word_list();
	std::vector<std::string> result;
	result.reserve(size);
	std::uniform_int_distribution<int> string_length_distribution(0, 10);
	//std::uniform_int_distribution<int> string_length_distribution(1, 3);
	std::uniform_int_distribution<size_t> word_picker(0, words.size() - 1);
	for (std::int64_t outer = 0; outer < size; ++outer)
	{
		std::string to_add;
		for (int inner = 0, end = string_length_distribution(randomness); inner < end; ++inner)
		{
			to_add += words[word_picker(randomness)];
		}
		result.push_back(std::move(to_add));
	}
	return result;
}

#endif

#if 0
struct benchmark_sort_value {
	std::array<uint8_t, 1016> array = {};
};
#define SORT_ON_FIRST_ONLY
typedef std::int64_t benchmark_sort_key;
#define NUM_SORT_KEYS 1
#if NUM_SORT_KEYS == 1
static std::vector<std::tuple<benchmark_sort_key, benchmark_sort_value>> SKA_SORT_NOINLINE create_radix_sort_data(std::mt19937_64 & randomness, int size)
{
	std::vector<std::tuple<benchmark_sort_key, benchmark_sort_value>> result;
	result.reserve(size);
	std::uniform_int_distribution<benchmark_sort_key> distribution(bounded::min_value<benchmark_sort_key>, bounded::max_value<benchmark_sort_key>);
	for (int i = 0; i < size; ++i)
	{
		result.push_back(distribution(randomness), benchmark_sort_value());
	}
	return result;
}
#elif NUM_SORT_KEYS == 2
static std::vector<std::tuple<std::pair<benchmark_sort_key, benchmark_sort_key>, benchmark_sort_value>> SKA_SORT_NOINLINE create_radix_sort_data(std::mt19937_64 & randomness, int size)
{
	std::vector<std::tuple<std::pair<benchmark_sort_key, benchmark_sort_key>, benchmark_sort_value>> result;
	result.reserve(size);
	std::uniform_int_distribution<benchmark_sort_key> distribution(bounded::min_value<benchmark_sort_key>, bounded::max_value<benchmark_sort_key>);
	for (int i = 0; i < size; ++i)
	{
		result.push_back(std::make_pair(distribution(randomness), distribution(randomness)), benchmark_sort_value());
	}
	return result;
}
#else
static std::vector<std::tuple<std::array<benchmark_sort_key, NUM_SORT_KEYS>, benchmark_sort_value>> SKA_SORT_NOINLINE create_radix_sort_data(std::mt19937_64 & randomness, int size)
{
	std::vector<std::tuple<std::array<benchmark_sort_key, NUM_SORT_KEYS>, benchmark_sort_value>> result;
	result.reserve(size);
	std::uniform_int_distribution<benchmark_sort_key> distribution(bounded::min_value<benchmark_sort_key>, bounded::max_value<benchmark_sort_key>);
	for (int i = 0; i < size; ++i)
	{
		std::array<benchmark_sort_key, NUM_SORT_KEYS> key;
		for (int i = 0; i < NUM_SORT_KEYS; ++i)
			key[i] = distribution(randomness);
		result.push_back(key, benchmark_sort_value());
	}
	return result;
}
#endif
#endif

static constexpr int profile_multiplier = 2;
static constexpr int max_profile_range = 1 << 20;

void benchmark_ska_sort_copy(benchmark::State & state, auto create) {
	auto randomness = std::mt19937_64(77342348);
	auto buffer = create(randomness, state.range(0));
	for (auto _ : state) {
		auto to_sort = create(randomness, state.range(0));
		using containers::data;
		benchmark::DoNotOptimize(data(to_sort));
		benchmark::DoNotOptimize(data(buffer));
		using containers::begin;
		using containers::end;
#ifdef SORT_ON_FIRST_ONLY
		ska_sort_copy(begin(to_sort), end(to_sort), begin(buffer), [](auto && a){
			using std::get;
			return get<0>(a);
		});
#else
		bool which = ska_sort_copy(begin(to_sort), end(to_sort), begin(buffer));
		if (which)
			assert(containers::is_sorted(buffer));
		else
			assert(containers::is_sorted(to_sort));
#endif
		benchmark::ClobberMemory();
	}
}

static void benchmark_std_sort(benchmark::State & state, auto create) {
	auto randomness = std::mt19937_64(77342348);
	create(randomness, state.range(0));
	for (auto _ : state) {
		auto to_sort = create(randomness, state.range(0));
		benchmark::DoNotOptimize(to_sort.data());
#ifdef SORT_ON_FIRST_ONLY
		std::sort(to_sort.begin(), to_sort.end(), [](auto && l, auto && r){ return std::get<0>(l) < std::get<0>(r); });
#else
		std::sort(to_sort.begin(), to_sort.end());
		assert(std::is_sorted(to_sort.begin(), to_sort.end()));
#endif
		benchmark::ClobberMemory();
	}
}

static void american_flag_sort(auto begin, auto end, auto && extract_key) {
	detail::inplace_radix_sort<bounded::max_value<std::ptrdiff_t>>(begin, end, extract_key);
}

static void american_flag_sort(auto begin, auto end) {
	american_flag_sort(begin, end, bounded::identity);
}

static void benchmark_american_flag_sort(benchmark::State & state, auto create) {
	auto randomness = std::mt19937_64(77342348);
	create(randomness, state.range(0));
	for (auto _ : state) {
		auto to_sort = create(randomness, state.range(0));
		benchmark::DoNotOptimize(to_sort.data());
#ifdef SORT_ON_FIRST_ONLY
		american_flag_sort(to_sort.begin(), to_sort.end(), [](auto && a) -> decltype(auto){ return std::get<0>(a); });
#else
		american_flag_sort(to_sort.begin(), to_sort.end());
		assert(std::is_sorted(to_sort.begin(), to_sort.end()));
#endif
		benchmark::ClobberMemory();
	}
}

static void benchmark_ska_sort(benchmark::State & state, auto create) {
	auto randomness = std::mt19937_64(77342348);
	create(randomness, state.range(0));
	for (auto _ : state) {
		auto to_sort = create(randomness, state.range(0));
		benchmark::DoNotOptimize(to_sort.data());
#ifdef SORT_ON_FIRST_ONLY
		ska_sort(to_sort.begin(), to_sort.end(), [](auto && a) -> decltype(auto){ return std::get<0>(a); });
#else
		ska_sort(to_sort.begin(), to_sort.end());
		assert(std::is_sorted(to_sort.begin(), to_sort.end()));
#endif
		benchmark::ClobberMemory();
	}
}

static void benchmark_inplace_radix_sort(benchmark::State & state, auto create) {
	auto randomness = std::mt19937_64(77342348);
	create(randomness, state.range(0));
	for (auto _ : state) {
		auto to_sort = create(randomness, state.range(0));
		benchmark::DoNotOptimize(to_sort.data());
#ifdef SORT_ON_FIRST_ONLY
		inplace_radix_sort(to_sort.begin(), to_sort.end(), [](auto && a) -> decltype(auto) { return std::get<0>(a); });
#else
		inplace_radix_sort(to_sort.begin(), to_sort.end());
		assert(std::is_sorted(to_sort.begin(), to_sort.end()));
#endif
		benchmark::ClobberMemory();
	}
}

static void benchmark_generation(benchmark::State & state, auto create) {
	auto randomness = std::mt19937_64(77342348);
	create(randomness, state.range(0));
	for (auto _ : state) {
		auto to_sort = create(randomness, state.range(0));
		benchmark::DoNotOptimize(to_sort.data());
		benchmark::ClobberMemory();
	}
}


static std::vector<std::int8_t> SKA_SORT_NOINLINE create_limited_radix_sort_data(std::mt19937_64 & randomness, std::int8_t range_end) {
	std::int8_t permutation[256];
	std::iota(permutation, permutation + 256, -128);
	std::shuffle(permutation, permutation + 256, randomness);
	std::vector<std::int8_t> result;
	std::size_t size = 2 * 1024 * 1024;
	result.reserve(size);
	std::uniform_int_distribution<std::int8_t> int_distribution(-128, range_end);
	for (size_t i = 0; i < size; ++i) {
		result.push_back(permutation[to_radix_sort_key(int_distribution(randomness))]);
	}
	return result;
}
static void benchmark_limited_generation(benchmark::State & state) {
	auto randomness = std::mt19937_64(77342348);
	for (auto _ : state) {
		auto to_sort = create_limited_radix_sort_data(randomness, state.range(0));
		benchmark::DoNotOptimize(to_sort.data());
		benchmark::ClobberMemory();
	}
}

#define LIMITED_RANGE() Arg(-128)->Arg(-127)->Arg(-120)->Arg(-96)->Arg(-64)->Arg(-32)->Arg(0)->Arg(32)->Arg(64)->Arg(96)->Arg(127)
BENCHMARK(benchmark_limited_generation)->LIMITED_RANGE();

static void benchmark_limited_inplace_sort(benchmark::State & state) {
	std::mt19937_64 randomness(77342348);
	for (auto _ : state) {
		auto to_sort = create_limited_radix_sort_data(randomness, state.range(0));
		benchmark::DoNotOptimize(to_sort.data());
		ska_sort(to_sort.begin(), to_sort.end());
		assert(std::is_sorted(to_sort.begin(), to_sort.end()));
		benchmark::ClobberMemory();
	}
}
BENCHMARK(benchmark_limited_inplace_sort)->LIMITED_RANGE();

#define REGISTER_INDIVIDUAL_BENCHMARK(name, runner, create) \
	do { \
		benchmark::RegisterBenchmark( \
			name, \
			[](auto & state) { runner(state, create); } \
		)->RangeMultiplier(profile_multiplier)->Range(profile_multiplier, max_profile_range); \
	} while (false)

#define REGISTER_SOME_BENCHMARKS(name, create) \
	do { \
		REGISTER_INDIVIDUAL_BENCHMARK("generation_" name, benchmark_generation, create); \
		REGISTER_INDIVIDUAL_BENCHMARK("american_flag_sort_" name, benchmark_american_flag_sort, create); \
		REGISTER_INDIVIDUAL_BENCHMARK("inplace_radix_sort_" name, benchmark_inplace_radix_sort, create); \
		REGISTER_INDIVIDUAL_BENCHMARK("ska_sort_" name, benchmark_ska_sort, create); \
		REGISTER_INDIVIDUAL_BENCHMARK("std_sort_" name, benchmark_std_sort, create); \
	} while (false)

#define REGISTER_BENCHMARK(name, create) \
	do { \
		REGISTER_SOME_BENCHMARKS(name, create); \
		REGISTER_INDIVIDUAL_BENCHMARK("ska_sort_copy_" name, benchmark_ska_sort_copy, create); \
	} while (false)

namespace {

auto create_simple_data = [](auto distribution) {
	return [=](auto & engine, std::int64_t const size) {
		return create_radix_sort_data(engine, size, distribution);
	};
};

// Technically this does not allow sharing a distribution for the same type, but
// that is probably not a serious problem for this use case
auto create_tuple_data = [](auto... distributions) {
	return [=](auto & engine, std::int64_t const size) mutable {
		return create_radix_sort_data(engine, size, [&](auto &) {
			return std::tuple(distributions(engine)...);
		});
	};
};

auto create_range_data = [](int max_size, auto generate) {
	auto size_distribution = std::uniform_int_distribution(0, max_size);
	return [=](auto & engine, std::int64_t const size) mutable {
		// TODO: Use integer range
		return create_radix_sort_data(engine, size, [&](auto &) {
			return generate(engine, size_distribution(engine));
		});
	};
};

using bounded::min_value;
using bounded::max_value;

static auto SKA_SORT_NOINLINE create_radix_sort_data_bool(std::mt19937_64 & engine, std::int64_t const size) {
	auto int_distribution = std::uniform_int_distribution<int>(0, 1);
	return create_radix_sort_data<containers::vector<bool>>(engine, size, [&](auto &) {
		return int_distribution(engine) != 0;
	});
}

void register_all_benchmarks() {
	REGISTER_BENCHMARK(
		"uint8",
		create_simple_data(std::uniform_int_distribution(min_value<std::uint8_t>, max_value<std::uint8_t>))
	);
	REGISTER_BENCHMARK(
		"int8",
		create_simple_data(std::uniform_int_distribution(min_value<std::int8_t>, max_value<std::int8_t>))
	);
	REGISTER_BENCHMARK(
		"uint16",
		create_simple_data(std::uniform_int_distribution(min_value<std::uint16_t>, max_value<std::uint16_t>))
	);
	REGISTER_BENCHMARK(
		"int16",
		create_simple_data(std::uniform_int_distribution(min_value<std::int16_t>, max_value<std::int16_t>))
	);
	REGISTER_BENCHMARK(
		"uint32",
		create_simple_data(std::uniform_int_distribution(min_value<std::uint32_t>, max_value<std::uint32_t>))
	);
	REGISTER_BENCHMARK(
		"int32",
		create_simple_data(std::uniform_int_distribution(min_value<std::int32_t>, max_value<std::int32_t>))
	);
	REGISTER_BENCHMARK(
		"uint64",
		create_simple_data(std::uniform_int_distribution(min_value<std::uint64_t>, max_value<std::uint64_t>))
	);
	REGISTER_BENCHMARK(
		"int64",
		create_simple_data(std::uniform_int_distribution(min_value<std::int64_t>, max_value<std::int64_t>))
	);
	REGISTER_BENCHMARK(
		"tuple_uint8_uint8_uint8_uint8",
		create_simple_data(
			[
				distribution = std::uniform_int_distribution(min_value<std::uint32_t>, max_value<std::uint32_t>)
			](auto & engine) mutable {
				auto result = distribution(engine);
				constexpr auto bits = std::numeric_limits<std::uint8_t>::digits;
				return std::array{
					static_cast<std::uint8_t>(result >> (bits * 0U)),
					static_cast<std::uint8_t>(result >> (bits * 1U)),
					static_cast<std::uint8_t>(result >> (bits * 2U)),
					static_cast<std::uint8_t>(result >> (bits * 3U)),
				};
			}
		)
	);
	REGISTER_BENCHMARK(
		"array_uint8_4",
		create_simple_data(
			[
				distribution = std::uniform_int_distribution(min_value<std::uint32_t>, max_value<std::uint32_t>)
			](auto & engine) mutable {
				auto result = distribution(engine);
				constexpr auto bits = std::numeric_limits<std::uint8_t>::digits;
				return std::array{
					static_cast<std::uint8_t>(result >> (bits * 0U)),
					static_cast<std::uint8_t>(result >> (bits * 1U)),
					static_cast<std::uint8_t>(result >> (bits * 2U)),
					static_cast<std::uint8_t>(result >> (bits * 3U)),
				};
			}
		)
	);

	REGISTER_BENCHMARK(
		"int8_0_1",
		create_simple_data(std::uniform_int_distribution<std::int8_t>(0, 1))
	);
	REGISTER_BENCHMARK(
		"geometric_int8",
		create_simple_data(std::geometric_distribution<std::uint8_t>(0.05))
	);
	REGISTER_BENCHMARK(
		"int64_int64_positive",
		create_tuple_data(
			std::uniform_int_distribution<std::int64_t>(),
			std::uniform_int_distribution<std::int64_t>()
		)
	);
	REGISTER_BENCHMARK(
		"int32_int32_int64",
		create_tuple_data(
			std::uniform_int_distribution(min_value<std::int32_t>, max_value<std::int32_t>),
			std::uniform_int_distribution(min_value<std::int32_t>, max_value<std::int32_t>),
			std::uniform_int_distribution(min_value<std::int64_t>, max_value<std::int64_t>)
		)
	);
	REGISTER_BENCHMARK(
		"bool_float",
		create_tuple_data(
			std::uniform_int_distribution<int>(0, 1),
			std::uniform_real_distribution<float>()
		)
	);
	REGISTER_SOME_BENCHMARKS(
		"vector_int_narrow",
		create_range_data(
			20,
			[value_distribution = std::uniform_int_distribution()](auto & engine, auto size) mutable {
				return create_radix_sort_data(engine, size, value_distribution);
			}
		)
	);
	REGISTER_SOME_BENCHMARKS(
		"vector_int_wide",
		create_range_data(128, [](auto &, auto size) {
			auto to_add = std::vector<int>(size);
			std::iota(to_add.begin(), to_add.end(), 0);
			return to_add;
		})
	);
	REGISTER_SOME_BENCHMARKS(
		"string",
		create_range_data(20, [char_distribution = std::uniform_int_distribution('a', 'z')](auto & engine, auto size) mutable {
			return create_radix_sort_data<std::string>(engine, size, char_distribution);
		})
	);
	REGISTER_SOME_BENCHMARKS(
		"vector_string",
		create_range_data(10, [](auto & engine, auto size) {
			auto function = [&, char_distribution = std::uniform_int_distribution('a', 'c')](auto &, auto inner_size) mutable {
				return create_radix_sort_data<std::string>(engine, inner_size, char_distribution);
			};
			return create_range_data(5, function)(engine, size);
		})
	);
}

auto run_unit_tests(int argc, char * * argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}

void run_benchmarks(int argc, char * * argv) {
	register_all_benchmarks();
	benchmark::Initialize(&argc, argv);
	benchmark::RunSpecifiedBenchmarks();
}

} // namespace

int main(int argc, char * * argv) {
	auto result = run_unit_tests(argc, argv);
	if (argc != 1) {
		run_benchmarks(argc, argv);
	}
	return result;
}