// Copyright David Stone 2015.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <containers/vector.hpp>

#include <containers/shrink_to_fit.hpp>

#include "../test_assert.hpp"
#include "test_sequence_container.hpp"

namespace {

using namespace bounded::literal;

static_assert(containers_test::test_sequence_container<containers::vector<bounded::test_int>>());

} // namespace
