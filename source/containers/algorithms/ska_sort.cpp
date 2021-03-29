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
#include <bounded/detail/overload.hpp>

#include <deque>
#include <random>
#include <vector>

#include "../../test_assert.hpp"
#include "../../test_int.hpp"

namespace {

using namespace containers;

constexpr auto default_copy = [](auto && value) { return to_radix_sort_key(OPERATORS_FORWARD(value)); };

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
	detail::inplace_radix_sort<1>(containers::range_view(source), function);
	BOUNDED_TEST(source == expected);
	return true;
}

template<typename T, std::size_t size>
constexpr bool test_sort_inplace(c_array<T, size> const & original_, c_array<T, size> const & expected_) {
	constexpr auto indexes = std::make_index_sequence<size>{};
	auto const original = copy_from_c_array(original_, indexes);
	auto const expected = copy_from_c_array(expected_, indexes);
	test_sort_inplace(original, expected, to_radix_sort_key);
	test_sort_inplace(original, expected, default_copy);
	return true;
}

constexpr bool test_sort(auto original, auto const & expected, auto function) {
	test_sort_copy(original, expected, function);
	test_sort_inplace(std::move(original), expected, std::move(function));
	return true;
}

constexpr bool test_sort(auto original, auto const & expected) {
	test_sort(original, expected, to_radix_sort_key);
	test_sort(std::move(original), expected, default_copy);
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
	test_sort(original, expected, containers::to_radix_sort_key);
	test_sort(original, expected, default_copy);
	return true;
}

template<typename T, std::size_t size>
constexpr bool test_common_prefix(c_array<T, size> const & source, std::ptrdiff_t const start_index, std::size_t const expected) {
	auto const prefix = containers::detail::common_prefix(
		containers::range_view(source),
		to_radix_sort_key,
		to_radix_sort_key,
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
		m_value(x)
	{
	}

	constexpr move_only(move_only &&) = default;
	constexpr move_only(move_only const &) = delete;
	constexpr move_only & operator=(move_only &&) & = default;
	constexpr move_only & operator=(move_only const &) & = default;

	friend constexpr auto operator<=>(move_only const &, move_only const &) = default;

	friend constexpr auto to_radix_sort_key(move_only const & arg) {
		return containers::to_radix_sort_key(arg.m_value.value());
	}

private:
	bounded::test_int m_value = 0;
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
	[](auto const & value) { return to_radix_sort_key(value); }
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
	[](auto const & value) { return to_radix_sort_key(value); }
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

static_assert(test_sort_inplace<std::string_view>(
	{
		"Hi",
		"There",
		"Hello",
		"World!",
		"Foo",
		"Bar",
		"Baz",
		"",
	},
	{
		"",
		"Bar",
		"Baz",
		"Foo",
		"Hello",
		"Hi",
		"There",
		"World!",
	}
));

// Work around https://gcc.gnu.org/bugzilla/show_bug.cgi?id=99018
#if !defined __GNUC__ or defined __clang__
static_assert(
	test_sort_inplace<containers::vector<int>>(
		{
			{{1, 2, 3}},
			{{1, 2, 2}},
			{{1, 3, 2}},
			{{2, 3, 2}},
			{{2, 3, 2, 4}},
			{{2, 3, 2, 4, 5}},
			{{3, 2, 4, 5}},
			{{1}},
			{{}},
		},
		{
			{{}},
			{{1}},
			{{1, 2, 2}},
			{{1, 2, 3}},
			{{1, 3, 2}},
			{{2, 3, 2}},
			{{2, 3, 2, 4}},
			{{2, 3, 2, 4, 5}},
			{{3, 2, 4, 5}},
		}
	)
);

static_assert(
	test_sort_inplace<containers::vector<containers::vector<bool>>>(
		{
			{{
				{{true}},
			}},
			{{
				{{true}},
				{{false}},
			}},
			{{
				{{}},
			}},
		},
		{
			{{
				{{}},
			}},
			{{
				{{true}},
			}},
			{{
				{{true}},
				{{false}},
			}},
		}
	)
);

static_assert(
	test_sort_inplace<containers::vector<bounded::tuple<bool>>>(
		{
			{{
				bounded::tuple(true),
				bounded::tuple(false),
			}},
			{{
				bounded::tuple(true),
			}},
		},
		{
			{{
				bounded::tuple(true),
			}},
			{{
				bounded::tuple(true),
				bounded::tuple(false),
			}},
		}
	)
);

static_assert(
	test_sort_inplace(
		{
			bounded::tuple(containers::vector({true, false})),
			bounded::tuple(containers::vector({true})),
		},
		{
			bounded::tuple(containers::vector({true})),
			bounded::tuple(containers::vector({true, false})),
		}
	)
);

#endif

struct wrapper {
	move_only value;
	friend constexpr auto operator==(wrapper const & lhs, wrapper const & rhs) -> bool {
		return to_radix_sort_key(lhs.value) == to_radix_sort_key(rhs.value);
	}
	friend constexpr auto operator<=>(wrapper const & lhs, wrapper const & rhs) {
		return to_radix_sort_key(lhs.value) <=> to_radix_sort_key(rhs.value);
	}
};

constexpr auto get_value_member = bounded::overload(
	[](wrapper const & w) { return bounded::tie(w.value); },
	[](move_only const & m) { return to_radix_sort_key(m); }
);
static_assert(test_sort_copy(
	containers::array<wrapper, 5>{
		wrapper{2},
		wrapper{3},
		wrapper{1},
		wrapper{5},
		wrapper{4},
	},
	containers::array<wrapper, 5>{
		wrapper{1},
		wrapper{2},
		wrapper{3},
		wrapper{4},
		wrapper{5},
	},
	get_value_member
));

} // namespace
