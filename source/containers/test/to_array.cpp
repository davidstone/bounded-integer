// Copyright David Stone 2023.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

export module containers.test.to_array;

import containers.algorithms.generate;
import containers.array;
import containers.range_value_t;
import containers.to_array;

import bounded;
import std_module;

namespace {
	
using namespace bounded::literal;
	
constexpr auto array_n = containers::make_array_n(6_bi, 5);
static_assert(std::same_as<containers::range_value_t<decltype(array_n)>, int>, "Incorrect type from make_array_n.");
static_assert(array_n == containers::array({5, 5, 5, 5, 5, 5}));

struct non_copyable {
	non_copyable() = default;
	non_copyable(non_copyable const &) = delete;
	non_copyable(non_copyable &&) = default;
};

constexpr auto array_non_copyable = containers::make_array_n(1_bi, non_copyable());
static_assert(array_non_copyable.size() == 1_bi);

constexpr auto array_empty = containers::make_array_n(0_bi, 2);
static_assert(array_empty.size() == 0_bi);

constexpr auto array_non_copyable_empty = containers::make_array_n(0_bi, non_copyable());
static_assert(array_non_copyable_empty.size() == 0_bi);

static_assert(containers::to_array({1, 2, 3}) == containers::array{1, 2, 3});
static_assert(containers::to_array(containers::generate_n(3_bi, [] { return 2; })) == containers::array{2, 2, 2});

} // namespace
