// Copyright David Stone 2015.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <containers/dynamic_array.hpp>

#include "test_sequence_container.hpp"

static_assert(containers_test::test_sequence_container<containers::dynamic_array<bounded::test_int>>());
