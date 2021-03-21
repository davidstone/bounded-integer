// Copyright David Stone 2019, Malte Skarupke 2016.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <containers/algorithms/ska_sort.hpp>
#include <containers/algorithms/sort.hpp>
#include <containers/array/array.hpp>
#include <containers/array/make_array.hpp>
#include <containers/c_array.hpp>
#include <containers/front_back.hpp>
#include <containers/push_back.hpp>
#include <containers/reserve_if_reservable.hpp>
#include <containers/static_vector/static_vector.hpp>
#include <containers/string.hpp>
#include <containers/vector.hpp>

#include <bounded/detail/tuple.hpp>
#include <bounded/copy.hpp>

#include <deque>
#include <random>
#include <vector>

#include "../../test_assert.hpp"

namespace {

using namespace containers;

template<typename T, std::size_t size, std::size_t... indexes>
constexpr auto copy_from_c_array(c_array<T, size> const & original, std::index_sequence<indexes...>) {
	return containers::array<std::remove_cvref_t<T>, sizeof...(indexes)>{original[indexes]...};
}

template<typename Container>
constexpr bool test_sort_copy(Container source, Container const & expected, auto function) {
	BOUNDED_TEST(containers::is_sorted(expected));
	auto other = Container();
	if constexpr (requires(Container c) { c.resize(size(c)); }) {
		other.resize(size(source));
	}
	bool const data_in_other = double_buffered_ska_sort(source, other, std::move(function));
	auto const & sorted = data_in_other ? other : source;
	BOUNDED_TEST(sorted == expected);
	return true;
}

constexpr bool test_sort_inplace(auto source, auto const & expected, auto function) {
	BOUNDED_TEST(containers::is_sorted(expected));
	detail::inplace_radix_sort<1>(begin(source), end(source), function);
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
constexpr bool test_common_prefix(c_array<T, size> const & source, std::ptrdiff_t const start_index, std::size_t const expected) {
	auto const prefix = containers::detail::common_prefix(
		std::begin(source),
		std::end(source),
		bounded::identity,
		bounded::identity,
		start_index
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
	containers::back(to_sort) = 255;
	return to_sort;
}();
static_assert(test_sort(
	uint8_256_items,
	uint8_256_items
));

static_assert(test_sort<std::uint16_t>(
	{5, 6, 19, 2, 5, 7, 0, 23, 6, 256, 255, 8, 99, 1024, 65535, 65534},
	{0, 2, 5, 5, 6, 6, 7, 8, 19, 23, 99, 255, 256, 1024, 65534, 65535}
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
	std::array<std::string_view, 8>{
		"Hi",
		"There",
		"Hello",
		"World!",
		"Foo",
		"Bar",
		"Baz",
		"",
	},
	std::array<std::string_view, 8>{
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

// Work around https://gcc.gnu.org/bugzilla/show_bug.cgi?id=99018
#if !defined __GNUC__ or defined __clang__
static_assert(
	test_sort_inplace(
		containers::vector({
			containers::vector({1, 2, 3}),
			containers::vector({1, 2, 2}),
			containers::vector({1, 3, 2}),
			containers::vector({2, 3, 2}),
			containers::vector({2, 3, 2, 4}),
			containers::vector({2, 3, 2, 4, 5}),
			containers::vector({3, 2, 4, 5}),
			containers::vector({1}),
			containers::vector<int>(),
		}),
		containers::vector({
			containers::vector<int>(),
			containers::vector({1}),
			containers::vector({1, 2, 2}),
			containers::vector({1, 2, 3}),
			containers::vector({1, 3, 2}),
			containers::vector({2, 3, 2}),
			containers::vector({2, 3, 2, 4}),
			containers::vector({2, 3, 2, 4, 5}),
			containers::vector({3, 2, 4, 5}),
		}),
		bounded::identity
	)
);

static_assert(
	test_sort_inplace(
		containers::vector({
			containers::vector({1, 2, 3}),
			containers::vector({1, 2, 2}),
			containers::vector({1, 3, 2}),
			containers::vector({2, 3, 2}),
			containers::vector({2, 3, 2, 4}),
			containers::vector({2, 3, 2, 4, 5}),
			containers::vector({3, 2, 4, 5}),
			containers::vector({1}),
			containers::vector<int>(),
		}),
		containers::vector({
			containers::vector<int>(),
			containers::vector({1}),
			containers::vector({1, 2, 2}),
			containers::vector({1, 2, 3}),
			containers::vector({1, 3, 2}),
			containers::vector({2, 3, 2}),
			containers::vector({2, 3, 2, 4}),
			containers::vector({2, 3, 2, 4, 5}),
			containers::vector({3, 2, 4, 5}),
		}),
		bounded::copy
	)
);

static_assert(
	test_sort_inplace(
		containers::vector({
			containers::vector<containers::string>({"hi", "there", "you"}),
			containers::vector<containers::string>({"are", "probably", "not", "going"}),
			containers::vector<containers::string>({"to", "pass"}),
			containers::vector<containers::string>({""}),
			containers::vector<containers::string>(),
			containers::vector<containers::string>({"this", "test", "the", "first"}),
			containers::vector<containers::string>({"time"}),
			containers::vector<containers::string>({"oh it did pass", "n", "e", "a", "t!"}),
			containers::vector<containers::string>({"hi", "there", "I", "added", "more", "tests"}),
			containers::vector<containers::string>({"hi", "there", "needed", "the", "same", "prefix"}),
		}),
		containers::vector({
			containers::vector<containers::string>(),
			containers::vector<containers::string>({""}),
			containers::vector<containers::string>({"are", "probably", "not", "going"}),
			containers::vector<containers::string>({"hi", "there", "I", "added", "more", "tests"}),
			containers::vector<containers::string>({"hi", "there", "needed", "the", "same", "prefix"}),
			containers::vector<containers::string>({"hi", "there", "you"}),
			containers::vector<containers::string>({"oh it did pass", "n", "e", "a", "t!"}),
			containers::vector<containers::string>({"this", "test", "the", "first"}),
			containers::vector<containers::string>({"time"}),
			containers::vector<containers::string>({"to", "pass"}),
		}),
		bounded::identity
	)
);

#if 0
static_assert(
	test_sort_inplace(
		containers::vector({
			containers::vector({"hi", "there", "you"}),
			containers::vector({"are", "probably", "not", "going"}),
			containers::vector({"to", "pass"}),
			containers::vector({""}),
			containers::vector(),
			containers::vector({"this", "test", "the", "first"}),
			containers::vector({"time"}),
			containers::vector({"oh it did pass", "n", "e", "a", "t!"}),
			containers::vector({"hi", "there", "I", "added", "more", "tests"}),
			containers::vector({"hi", "there", "needed", "the", "same", "prefix"}),
		}),
		containers::vector({
			containers::vector(),
			containers::vector({""}),
			containers::vector({"are", "probably", "not", "going"}),
			containers::vector({"hi", "there", "I", "added", "more", "tests"}),
			containers::vector({"hi", "there", "needed", "the", "same", "prefix"}),
			containers::vector({"hi", "there", "you"}),
			containers::vector({"oh it did pass", "n", "e", "a", "t!"}),
			containers::vector({"this", "test", "the", "first"}),
			containers::vector({"time"}),
			containers::vector({"to", "pass"}),
		}),
		identity_by_value
	)
);
#endif

static_assert(
	test_sort_inplace(
		containers::vector<bounded::tuple<containers::string, containers::string>>({
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
		}),
		containers::vector<bounded::tuple<containers::string, containers::string>>({
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
		}),
		bounded::identity
	)
);

#if 0
static_assert(
	test_sort_inplace(
		containers::vector<bounded::tuple<containers::string, containers::string>>({
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
		}),
		containers::vector<bounded::tuple<containers::string, containers::string>>({
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
		}),
		identity_by_value
	)
);
#endif
#endif

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

} // namespace
