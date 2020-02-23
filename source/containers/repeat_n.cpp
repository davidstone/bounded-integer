// Copyright David Stone 2019.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <containers/repeat_n.hpp>

#include <containers/algorithms/compare.hpp>
#include <containers/array/array.hpp>

namespace {

using namespace bounded::literal;

static_assert(containers::equal(containers::repeat_default_n<int>(0_bi), containers::array<int, 0>{}));
static_assert(containers::equal(containers::repeat_default_n<int>(5_bi), containers::array{0, 0, 0, 0, 0}));
static_assert(containers::equal(containers::repeat_n(0_bi, 3), containers::array<int, 0>{}));
static_assert(containers::equal(containers::repeat_n(2_bi, 3), containers::array{3, 3}));

} // namespace