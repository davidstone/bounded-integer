// Copyright David Stone 2024.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

module;

#include <bounded/assert.hpp>

export module containers.test.algorithms.find;

import containers.algorithms.find;
import containers.array;
import containers.begin_end;
import containers.c_array;
import containers.size;

import bounded;
import std_module;

using namespace bounded::literal;

template<std::size_t size>
constexpr auto test(auto const index, containers::c_array<int, size> && a) -> bool {
	auto it = containers::find(a, 5);
	BOUNDED_ASSERT(index == containers::size(a) or *it == 5);
	return it - containers::begin(a) == index;
}

constexpr auto empty = containers::array<int, 0_bi>();
static_assert(containers::find(empty, 5) == containers::end(empty));
static_assert(test(1_bi, {0}));
static_assert(test(0_bi, {5}));
static_assert(test(2_bi, {0, 0}));
static_assert(test(1_bi, {0, 5}));
static_assert(test(0_bi, {5, 0}));
static_assert(test(0_bi, {5, 5}));


template<std::size_t size>
constexpr auto test_backward(auto const index, containers::c_array<int, size> && a) -> bool {
	auto it = containers::find_last(a, 5);
	BOUNDED_ASSERT(index == containers::size(a) or *it == 5);
	return it - containers::begin(a) == index;
}

static_assert(containers::find_last(empty, 5) == containers::end(empty));
static_assert(test_backward(1_bi, {0}));
static_assert(test_backward(0_bi, {5}));
static_assert(test_backward(2_bi, {0, 0}));
static_assert(test_backward(1_bi, {0, 5}));
static_assert(test_backward(0_bi, {5, 0}));
static_assert(test_backward(1_bi, {5, 5}));