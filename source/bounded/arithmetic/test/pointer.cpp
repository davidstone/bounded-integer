// Copyright David Stone 2024.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

export module bounded.arithmetic.test.pointer;

import bounded.arithmetic.pointer;

import bounded.bounded_integer;
import bounded.literal;

import std_module;

namespace {
using namespace bounded::literal;

constexpr int array[5] = {};

static_assert(std::begin(array) + 0_bi == std::begin(array));
static_assert(std::begin(array) + 5_bi == std::end(array));

} // namespace