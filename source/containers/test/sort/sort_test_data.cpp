// Copyright David Stone 2021.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

module;

#include <bounded/assert.hpp>

#include <operators/forward.hpp>

export module containers.test.sort.sort_test_data;

import containers.algorithms.sort.is_sorted;
import containers.algorithms.sort.to_radix_sort_key;

import containers.array;
import containers.back;
import containers.c_array;
import containers.static_vector;
import containers.vector;

import bounded;
import bounded.test_int;
import numeric_traits;
import tv;
import std_module;

namespace containers_test {

using namespace bounded::literal;

struct default_copy_t {
	static constexpr auto operator()(auto && value) {
		return containers::to_radix_sort_key(OPERATORS_FORWARD(value));
	}
};
export constexpr auto default_copy = default_copy_t();

export template<typename Container>
struct sort_test_data {
	constexpr sort_test_data(Container input_, Container expected_):
		input(std::move(input_)),
		expected(std::move(expected_))
	{
		BOUNDED_ASSERT(containers::is_sorted(expected));
	}
	Container input;
	Container expected;
};

export constexpr auto bool_0 = sort_test_data(
	containers::array<bool, 0_bi>(),
	containers::array<bool, 0_bi>()
);

export constexpr auto bool_1 = containers::array{
	sort_test_data(
		containers::array{false},
		containers::array{false}
	),
	sort_test_data(
		containers::array{true},
		containers::array{true}
	)
};

export constexpr auto bool_2 = containers::array{
	sort_test_data(
		containers::array{false, false},
		containers::array{false, false}
	),
	sort_test_data(
		containers::array{false, true},
		containers::array{false, true}
	),
	sort_test_data(
		containers::array{true, false},
		containers::array{false, true}
	),
	sort_test_data(
		containers::array{true, true},
		containers::array{true, true}
	)
};

export constexpr auto bool_3 = containers::array{
	sort_test_data(
		containers::array{false, false, false},
		containers::array{false, false, false}
	),
	sort_test_data(
		containers::array{false, false, true},
		containers::array{false, false, true}
	),
	sort_test_data(
		containers::array{false, true, false},
		containers::array{false, false, true}
	),
	sort_test_data(
		containers::array{true, false, false},
		containers::array{false, false, true}
	),
	sort_test_data(
		containers::array{false, true, true},
		containers::array{false, true, true}
	),
	sort_test_data(
		containers::array{true, false, true},
		containers::array{false, true, true}
	),
	sort_test_data(
		containers::array{true, true, false},
		containers::array{false, true, true}
	),
	sort_test_data(
		containers::array{true, true, true},
		containers::array{true, true, true}
	)
};

export constexpr auto bool_many = sort_test_data(
	containers::array{true, false, true, false, true, false, false, true, true, false},
	containers::array{false, false, false, false, false, true, true, true, true, true}
);

consteval auto operator""_u8(unsigned long long const x) {
	return static_cast<std::uint8_t>(x);
}
consteval auto operator""_u16(unsigned long long const x) {
	return static_cast<std::uint16_t>(x);
}
consteval auto operator""_u32(unsigned long long const x) {
	return static_cast<std::uint32_t>(x);
}
consteval auto operator""_u64(unsigned long long const x) {
	return static_cast<std::uint64_t>(x);
}

export constexpr auto uint8_0 = containers::array{
	sort_test_data(
		containers::static_vector<std::uint8_t, 2_bi>(),
		containers::static_vector<std::uint8_t, 2_bi>()
	)
};

export constexpr auto uint8_1 = containers::array{
	sort_test_data(
		containers::array{1_u8},
		containers::array{1_u8}
	)
};

export constexpr auto uint8_2 = containers::array{
	sort_test_data(
		containers::array{1_u8, 1_u8},
		containers::array{1_u8, 1_u8}
	),
	sort_test_data(
		containers::array{1_u8, 2_u8},
		containers::array{1_u8, 2_u8}
	),
	sort_test_data(
		containers::array{2_u8, 1_u8},
		containers::array{1_u8, 2_u8}
	),
};

export constexpr auto uint8_3 = containers::array{
	sort_test_data(
		containers::array{1_u8, 1_u8, 1_u8},
		containers::array{1_u8, 1_u8, 1_u8}
	),
	sort_test_data(
		containers::array{1_u8, 1_u8, 2_u8},
		containers::array{1_u8, 1_u8, 2_u8}
	),
	sort_test_data(
		containers::array{1_u8, 2_u8, 1_u8},
		containers::array{1_u8, 1_u8, 2_u8}
	),
	sort_test_data(
		containers::array{2_u8, 1_u8, 1_u8},
		containers::array{1_u8, 1_u8, 2_u8}
	),
	sort_test_data(
		containers::array{1_u8, 2_u8, 2_u8},
		containers::array{1_u8, 2_u8, 2_u8}
	),
	sort_test_data(
		containers::array{2_u8, 1_u8, 2_u8},
		containers::array{1_u8, 2_u8, 2_u8}
	),
	sort_test_data(
		containers::array{2_u8, 2_u8, 1_u8},
		containers::array{1_u8, 2_u8, 2_u8}
	),
	sort_test_data(
		containers::array{1_u8, 2_u8, 3_u8},
		containers::array{1_u8, 2_u8, 3_u8}
	),
	sort_test_data(
		containers::array{1_u8, 3_u8, 2_u8},
		containers::array{1_u8, 2_u8, 3_u8}
	),
	sort_test_data(
		containers::array{2_u8, 1_u8, 3_u8},
		containers::array{1_u8, 2_u8, 3_u8}
	),
	sort_test_data(
		containers::array{2_u8, 3_u8, 1_u8},
		containers::array{1_u8, 2_u8, 3_u8}
	),
	sort_test_data(
		containers::array{3_u8, 1_u8, 2_u8},
		containers::array{1_u8, 2_u8, 3_u8}
	),
	sort_test_data(
		containers::array{3_u8, 2_u8, 1_u8},
		containers::array{1_u8, 2_u8, 3_u8}
	),
};

export constexpr auto uint8_many = sort_test_data(
	containers::array{5_u8, 6_u8, 19_u8, 2_u8, 5_u8, 0_u8, 7_u8, 23_u8, 6_u8, 8_u8, 99_u8},
	containers::array{0_u8, 2_u8, 5_u8, 5_u8, 6_u8, 6_u8, 7_u8, 8_u8, 19_u8, 23_u8, 99_u8}
);


export constexpr auto uint8_256 = []{
	auto to_sort = containers::make_array_n(bounded::constant<256>, 254_u8);
	containers::back(to_sort) = 255;
	return sort_test_data(to_sort, to_sort);
}();


export constexpr auto uint16_many = sort_test_data(
	containers::array{
		5_u16,
		6_u16,
		19_u16,
		2_u16,
		5_u16,
		7_u16,
		0_u16,
		23_u16,
		6_u16,
		256_u16,
		255_u16,
		8_u16,
		99_u16,
		1024_u16,
		65535_u16,
		65534_u16
	},
	containers::array{
		0_u16,
		2_u16,
		5_u16,
		5_u16,
		6_u16,
		6_u16,
		7_u16,
		8_u16,
		19_u16,
		23_u16,
		99_u16,
		255_u16,
		256_u16,
		1024_u16,
		65534_u16,
		65535_u16
	}
);

export constexpr auto uint32_many = sort_test_data(
	containers::array{
		5_u32,
		6_u32,
		19_u32,
		2_u32,
		5_u32,
		7_u32,
		0_u32,
		23_u32,
		6_u32,
		256_u32,
		255_u32,
		8_u32,
		99_u32,
		1024_u32,
		65536_u32,
		65535_u32,
		65534_u32,
		1000000_u32,
		numeric_traits::max_value<std::uint32_t>
	},
	containers::array{
		0_u32,
		2_u32,
		5_u32,
		5_u32,
		6_u32,
		6_u32,
		7_u32,
		8_u32,
		19_u32,
		23_u32,
		99_u32,
		255_u32,
		256_u32,
		1024_u32,
		65534_u32,
		65535_u32,
		65536_u32,
		1000000_u32,
		numeric_traits::max_value<std::uint32_t>
	}
);

export constexpr auto uint64_many = sort_test_data(
	containers::array{
		5_u64,
		6_u64,
		19_u64,
		2_u64,
		5_u64,
		7_u64,
		0_u64,
		static_cast<std::uint64_t>(numeric_traits::max_value<std::uint32_t>) + 1_u64,
		1'000'000'000'000_u64,
		numeric_traits::max_value<std::uint64_t>,
		23_u64,
		6_u64,
		256_u64,
		255_u64,
		8_u64,
		99_u64,
		1'024_u64,
		65'536_u64,
		65'535_u64,
		65'534_u64,
		1'000'000_u64,
		numeric_traits::max_value<std::uint32_t>,
	},
	containers::array{
		0_u64,
		2_u64,
		5_u64,
		5_u64,
		6_u64,
		6_u64,
		7_u64,
		8_u64,
		19_u64,
		23_u64,
		99_u64,
		255_u64,
		256_u64,
		1'024_u64,
		65'534_u64,
		65'535_u64,
		65'536_u64,
		1'000'000_u64,
		numeric_traits::max_value<std::uint32_t>,
		static_cast<std::uint64_t>(numeric_traits::max_value<std::uint32_t>) + 1_u64,
		1'000'000'000'000_u64,
		numeric_traits::max_value<std::uint64_t>,
	}
);


export constexpr auto tuple_many = sort_test_data(
	containers::array{
		tv::tuple(true, 5_u16, true),
		tv::tuple(true, 5_u16, false),
		tv::tuple(false, 6_u16, false),
		tv::tuple(true, 7_u16, true),
		tv::tuple(true, 4_u16, false),
		tv::tuple(false, 4_u16, true),
		tv::tuple(false, 5_u16, false),
	},
	containers::array{
		tv::tuple(false, 4_u16, true),
		tv::tuple(false, 5_u16, false),
		tv::tuple(false, 6_u16, false),
		tv::tuple(true, 4_u16, false),
		tv::tuple(true, 5_u16, false),
		tv::tuple(true, 5_u16, true),
		tv::tuple(true, 7_u16, true),
	}
);


export constexpr auto array_uint8_1_1 = sort_test_data(
	containers::array<std::uint8_t, 1_bi, 1_bi>{{
		{1},
	}},
	containers::array<std::uint8_t, 1_bi, 1_bi>{{
		{1},
	}}
);

export constexpr auto array_uint8_1_2 = containers::array{
	sort_test_data(
		containers::array<std::uint8_t, 2_bi, 1_bi>{{
			{1},
			{1},
		}},
		containers::array<std::uint8_t, 2_bi, 1_bi>{{
			{1},
			{1},
		}}
	),
	sort_test_data(
		containers::array<std::uint8_t, 2_bi, 1_bi>{{
			{1},
			{2},
		}},
		containers::array<std::uint8_t, 2_bi, 1_bi>{{
			{1},
			{2},
		}}
	),
	sort_test_data(
		containers::array<std::uint8_t, 2_bi, 1_bi>{{
			{2},
			{1},
		}},
		containers::array<std::uint8_t, 2_bi, 1_bi>{{
			{1},
			{2},
		}}
	)
};

export constexpr auto array_uint8_1_3_one_value = sort_test_data(
	containers::array<std::uint8_t, 3_bi, 1_bi>{{
		{1},
		{1},
		{1},
	}},
	containers::array<std::uint8_t, 3_bi, 1_bi>{{
		{1},
		{1},
		{1},
	}}
);

export constexpr auto array_uint8_1_3_two_values = containers::array{
	sort_test_data(
		containers::array<std::uint8_t, 3_bi, 1_bi>{{
			{1},
			{1},
			{2},
		}},
		containers::array<std::uint8_t, 3_bi, 1_bi>{{
			{1},
			{1},
			{2},
		}}
	),
	sort_test_data(
		containers::array<std::uint8_t, 3_bi, 1_bi>{{
			{1},
			{2},
			{1},
		}},
		containers::array<std::uint8_t, 3_bi, 1_bi>{{
			{1},
			{1},
			{2},
		}}
	),
	sort_test_data(
		containers::array<std::uint8_t, 3_bi, 1_bi>{{
			{2},
			{1},
			{1},
		}},
		containers::array<std::uint8_t, 3_bi, 1_bi>{{
			{1},
			{1},
			{2},
		}}
	),
	sort_test_data(
		containers::array<std::uint8_t, 3_bi, 1_bi>{{
			{1},
			{2},
			{2},
		}},
		containers::array<std::uint8_t, 3_bi, 1_bi>{{
			{1},
			{2},
			{2},
		}}
	),
	sort_test_data(
		containers::array<std::uint8_t, 3_bi, 1_bi>{{
			{2},
			{1},
			{2},
		}},
		containers::array<std::uint8_t, 3_bi, 1_bi>{{
			{1},
			{2},
			{2},
		}}
	),
	sort_test_data(
		containers::array<std::uint8_t, 3_bi, 1_bi>{{
			{2},
			{2},
			{1},
		}},
		containers::array<std::uint8_t, 3_bi, 1_bi>{{
			{1},
			{2},
			{2},
		}}
	)
};

export constexpr auto array_uint8_1_3_three_values = containers::array{
	sort_test_data(
		containers::array<std::uint8_t, 3_bi, 1_bi>{{
			{1},
			{2},
			{3},
		}},
		containers::array<std::uint8_t, 3_bi, 1_bi>{{
			{1},
			{2},
			{3},
		}}
	),
	sort_test_data(
		containers::array<std::uint8_t, 3_bi, 1_bi>{{
			{1},
			{3},
			{2},
		}},
		containers::array<std::uint8_t, 3_bi, 1_bi>{{
			{1},
			{2},
			{3},
		}}
	),
	sort_test_data(
		containers::array<std::uint8_t, 3_bi, 1_bi>{{
			{2},
			{1},
			{3},
		}},
		containers::array<std::uint8_t, 3_bi, 1_bi>{{
			{1},
			{2},
			{3},
		}}
	),
	sort_test_data(
		containers::array<std::uint8_t, 3_bi, 1_bi>{{
			{2},
			{3},
			{1},
		}},
		containers::array<std::uint8_t, 3_bi, 1_bi>{{
			{1},
			{2},
			{3},
		}}
	),
	sort_test_data(
		containers::array<std::uint8_t, 3_bi, 1_bi>{{
			{3},
			{1},
			{2},
		}},
		containers::array<std::uint8_t, 3_bi, 1_bi>{{
			{1},
			{2},
			{3},
		}}
	),
	sort_test_data(
		containers::array<std::uint8_t, 3_bi, 1_bi>{{
			{3},
			{2},
			{1},
		}},
		containers::array<std::uint8_t, 3_bi, 1_bi>{{
			{1},
			{2},
			{3},
		}}
	)
};


export constexpr auto array_uint8_4_many = sort_test_data(
	containers::array<std::uint8_t, 6_bi, 4_bi>{{
		{1, 2, 3, 4},
		{0, 3, 4, 5},
		{1, 1, 2, 2},
		{1, 2, 2, 4},
		{1, 2, 2, 3},
		{0, 3, 4, 4},
	}},
	containers::array<std::uint8_t, 6_bi, 4_bi>{{
		{0, 3, 4, 4},
		{0, 3, 4, 5},
		{1, 1, 2, 2},
		{1, 2, 2, 3},
		{1, 2, 2, 4},
		{1, 2, 3, 4},
	}}
);

export constexpr auto array_uint16_many = sort_test_data(
	containers::array<std::uint16_t, 2_bi, 2_bi>{{
		{0, 5},
		{0, 4},
	}},
	containers::array<std::uint16_t, 2_bi, 2_bi>{{
		{0, 4},
		{0, 5},
	}}
);

using namespace std::string_view_literals;
export constexpr auto strings = sort_test_data(
	containers::array{
		"Hi"sv,
		"There"sv,
		"Hello"sv,
		"World!"sv,
		"Foo"sv,
		"Bar"sv,
		"Baz"sv,
		""sv,
	},
	containers::array{
		""sv,
		"Bar"sv,
		"Baz"sv,
		"Foo"sv,
		"Hello"sv,
		"Hi"sv,
		"There"sv,
		"World!"sv,
	}
);


export constexpr auto make_vector_int_9() {
	return sort_test_data(
		containers::to_array<containers::vector<int>>({
			{{1, 2, 3}},
			{{1, 2, 2}},
			{{1, 3, 2}},
			{{2, 3, 2}},
			{{2, 3, 2, 4}},
			{{2, 3, 2, 4, 5}},
			{{3, 2, 4, 5}},
			{{1}},
			{{}},
		}),
		containers::to_array<containers::vector<int>>({
			{{}},
			{{1}},
			{{1, 2, 2}},
			{{1, 2, 3}},
			{{1, 3, 2}},
			{{2, 3, 2}},
			{{2, 3, 2, 4}},
			{{2, 3, 2, 4, 5}},
			{{3, 2, 4, 5}},
		})
	);
}

export constexpr auto make_vector_vector() {
	return sort_test_data(
		containers::to_array<containers::vector<containers::vector<bool>>>({
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
		}),
		containers::to_array<containers::vector<containers::vector<bool>>>({
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
		})
	);
}

export constexpr auto make_vector_tuple() {
	return sort_test_data(
		containers::to_array<containers::vector<tv::tuple<bool>>>({
			{{
				tv::tuple(true),
				tv::tuple(false),
			}},
			{{
				tv::tuple(true),
			}},
		}),
		containers::to_array<containers::vector<tv::tuple<bool>>>({
			{{
				tv::tuple(true),
			}},
			{{
				tv::tuple(true),
				tv::tuple(false),
			}},
		})
	);
}

export constexpr auto make_tuple_vector() {
	return sort_test_data(
		containers::to_array<tv::tuple<containers::vector<bool>>>({
			tv::tuple(containers::vector({true, false})),
			tv::tuple(containers::vector({true})),
		}),
		containers::to_array<tv::tuple<containers::vector<bool>>>({
			tv::tuple(containers::vector({true})),
			tv::tuple(containers::vector({true, false})),
		})
	);
}


export constexpr auto tuple_tuple = sort_test_data(
	containers::to_array<tv::tuple<bool, tv::tuple<tv::tuple<bool, bool>, bool>>>({
		{true, tv::tuple(tv::tuple(false, true), true)},
		{false, tv::tuple(tv::tuple(false, true), true)},
		{false, tv::tuple(tv::tuple(false, true), true)},
		{false, tv::tuple(tv::tuple(false, false), true)},
		{false, tv::tuple(tv::tuple(true, false), true)},
		{false, tv::tuple(tv::tuple(true, false), true)},
		{false, tv::tuple(tv::tuple(true, false), false)},
	}),
	containers::to_array<tv::tuple<bool, tv::tuple<tv::tuple<bool, bool>, bool>>>({
		{false, tv::tuple(tv::tuple(false, false), true)},
		{false, tv::tuple(tv::tuple(false, true), true)},
		{false, tv::tuple(tv::tuple(false, true), true)},
		{false, tv::tuple(tv::tuple(true, false), false)},
		{false, tv::tuple(tv::tuple(true, false), true)},
		{false, tv::tuple(tv::tuple(true, false), true)},
		{true, tv::tuple(tv::tuple(false, true), true)},
	})
);


struct move_only {
	move_only() = default;

	constexpr explicit move_only(int x):
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
	bounded_test::integer m_value = 0;
};


export constexpr auto make_move_only() {
	return sort_test_data(
		containers::array{
			move_only(5),
			move_only(1234567),
			move_only(-1000),
		},
		containers::array{
			move_only(-1000),
			move_only(5),
			move_only(1234567),
		}
	);
}

struct wrapper {
	wrapper() = default;
	constexpr explicit wrapper(int x):
		value(x)
	{
	}

	friend constexpr auto operator==(wrapper const & lhs, wrapper const & rhs) -> bool {
		return to_radix_sort_key(lhs.value) == to_radix_sort_key(rhs.value);
	}
	friend constexpr auto operator<=>(wrapper const & lhs, wrapper const & rhs) {
		return to_radix_sort_key(lhs.value) <=> to_radix_sort_key(rhs.value);
	}

	move_only value;
};

// TODO: make this a lambda again
struct wrapper_to_radix_sort_key {
	static constexpr auto operator()(wrapper const & w) {
		return tv::tie(w.value);
	}
};

export constexpr auto get_value_member = tv::overload(
	wrapper_to_radix_sort_key(),
	containers::to_radix_sort_key
);

export constexpr auto make_wrapper() {
	return sort_test_data(
		containers::array{
			wrapper(2),
			wrapper(3),
			wrapper(1),
		},
		containers::array{
			wrapper(1),
			wrapper(2),
			wrapper(3),
		}
	);
}

} // namespace containers_test