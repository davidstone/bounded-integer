// Copyright David Stone 2015.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <containers/algorithms/accumulate.hpp>

#include <containers/array/array.hpp>

namespace {

using namespace bounded::literal;

static_assert(containers::sum(containers::array{0_bi, 3_bi, 2_bi, 3_bi, 5_bi}) == (0_bi + 3_bi + 2_bi + 3_bi + 5_bi));
static_assert(containers::sum(containers::array({0.0, 1.0, 2.0})) == (0.0 + 1.0 + 2.0));

} // namespace