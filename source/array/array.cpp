// Copyright David Stone 2015.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <containers/array/array.hpp>
#include <containers/array/make_array.hpp>
#include <containers/algorithms/accumulate.hpp>
#include <containers/algorithms/count.hpp>

#include <bounded/integer.hpp>

namespace {
using namespace bounded::literal;

constexpr auto size = 5;
using array_type = containers::array<int, size>;
constexpr array_type a = {{}};
static_assert(containers::size(a) == size, "Incorrect size.");
static_assert(a[0_bi] == 0, "Incorrect value.");
static_assert(at(a, size - 1) == 0, "Incorrect value with at.");
static_assert(a.end() - a.begin() == size, "Incorrect difference type.");


constexpr containers::array<int, 0> empty_array = {{}};
static_assert(empty_array.begin() == empty_array.end(), "Empty array.");
// static_assert(empty_array[0_bi] == 0, "Should not compile.");

constexpr auto array = containers::make_array(0_bi, 3_bi, 2_bi, 3_bi, 5_bi);
static_assert(containers::count(array.begin(), array.end(), 3_bi) == 2_bi);
static_assert(containers::count(array.begin(), array.end(), 2_bi) == 1_bi);
static_assert(containers::count(array.begin(), array.end(), 7_bi) == 0_bi);

struct true_function {
	template<typename... Args>
	constexpr auto operator()(Args && ...) noexcept { return true; }
};
static_assert(containers::count_if(array.begin(), array.end(), true_function{}) == containers::size(array));


static_assert(containers::accumulate(array.begin(), array.end()) == (0_bi + 3_bi + 2_bi + 3_bi + 5_bi));
static_assert(containers::accumulate(array.begin(), array.end(), 10_bi) == (10_bi + 0_bi + 3_bi + 2_bi + 3_bi + 5_bi));

constexpr auto double_array = containers::make_array(0.0, 1.0, 2.0);

static_assert(containers::accumulate(double_array.begin(), double_array.end()) == (0.0 + 1.0 + 2.0));

}	// namespace

auto main() -> int {
}
