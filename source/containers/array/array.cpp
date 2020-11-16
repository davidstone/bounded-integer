// Copyright David Stone 2017.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <containers/array/array.hpp>
#include <containers/array/make_array.hpp>
#include <containers/algorithms/accumulate.hpp>
#include <containers/algorithms/count.hpp>
#include <containers/at.hpp>

#include <bounded/integer.hpp>

namespace {
using namespace bounded::literal;

constexpr auto size = 5;
using array_type = containers::array<int, size>;
constexpr array_type a = {};
static_assert(containers::size(a) == size, "Incorrect size.");
static_assert(a[0_bi] == 0, "Incorrect value.");
static_assert(containers::at(a, size - 1) == 0, "Incorrect value with at.");
static_assert(end(a) - begin(a) == size, "Incorrect difference type.");

static_assert(std::is_standard_layout_v<containers::array<int, 0>>);
static_assert(std::is_trivial_v<containers::array<int, 0>>);
static_assert(std::is_empty_v<containers::array<int, 0>>);
constexpr containers::array<int, 0> empty_array = {};
static_assert(begin(empty_array) == end(empty_array), "Empty array.");

constexpr auto array = containers::make_array(0_bi, 3_bi, 2_bi, 3_bi, 5_bi);
static_assert(containers::count(array, 3_bi) == 2_bi);
static_assert(containers::count(array, 2_bi) == 1_bi);
static_assert(containers::count(array, 7_bi) == 0_bi);

constexpr auto true_function = [](auto const &) { return true; };

using namespace bounded::literal;

static_assert(containers::count_if(array, true_function) == containers::size(array));

static_assert(containers::sum(array) == (0_bi + 3_bi + 2_bi + 3_bi + 5_bi));

constexpr auto double_array = containers::make_array(0.0, 1.0, 2.0);

static_assert(containers::sum(double_array) == (0.0 + 1.0 + 2.0));

}	// namespace
