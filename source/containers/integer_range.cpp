// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <containers/integer_range.hpp>
#include <containers/algorithms/accumulate.hpp>
#include <bounded/detail/literal.hpp>

namespace {
using namespace ::bounded::literal;

constexpr auto x = containers::integer_range(10_bi);
static_assert(
	*begin(x) == 0_bi,
	"Incorrect initial value of range."
);
static_assert(
	front(x) == 0_bi,
	"Incorrect initial value of range."
);
static_assert(
	back(x) == 9_bi,
	"Incorrect initial value of range."
);

static_assert(
	containers::integer_range(5_bi, 12_bi)[2_bi] == 7_bi,
	"Incorrect indexed value of range that does not start with 0."
);

static_assert(
	size(containers::integer_range(0_bi, 0_bi)) == 0_bi,
	"Incorrect size of empty range."
);
static_assert(
	empty(containers::integer_range(-5_bi, -5_bi)),
	"Incorrect size of empty range."
);

static_assert(
	size(containers::integer_range(
		static_cast<bounded::integer<1, 2>>(1_bi),
		static_cast<bounded::integer<3, 5>>(3_bi)
	)) == 2_bi,
	"Incorrect size of range with non-overlapping and possibly variable begin and end."
);


constexpr auto first = bounded::integer<-4, 1>(-3);
constexpr auto last = bounded::integer<0, 1>(1);
constexpr auto range = containers::integer_range(first, last);

static_assert(containers::accumulate(range) == -6_bi);

static_assert(containers::accumulate(containers::integer_range(4_bi, 10_bi, 2_bi)) == 18_bi);

// Ensure it works with a range-based for loop
static_assert([]{
	for (auto const n [[maybe_unused]] : range) {
	}
	return true;
}());

}	// namespace
