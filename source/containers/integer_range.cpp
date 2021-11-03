// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <containers/integer_range.hpp>

#include <containers/algorithms/compare.hpp>
#include <containers/array.hpp>

#include <bounded/detail/literal.hpp>

namespace {
using namespace ::bounded::literal;

static_assert(containers::equal(
	containers::integer_range(0_bi),
	containers::array<bounded::integer<0, 0>, 0>{}
));
static_assert(containers::equal(
	containers::integer_range(3_bi),
	containers::array{0_bi, 1_bi, 2_bi}
));
static_assert(containers::equal(
	containers::integer_range(2_bi, 4_bi),
	containers::array{2_bi, 3_bi}
));
static_assert(containers::equal(
	containers::integer_range(2_bi, 6_bi, 2_bi),
	containers::array{2_bi, 4_bi}
));

static_assert(containers::integer_range(3_bi)[1_bi] == 1_bi);

static_assert(containers::equal(
	containers::integer_range(bounded::integer<1, 2>(1_bi), bounded::integer<3, 5>(3_bi)),
	containers::array{1_bi, 2_bi}
));

static_assert([]{
	for (auto const n [[maybe_unused]] : containers::integer_range(1_bi)) {
	}
	return true;
}());

} // namespace
