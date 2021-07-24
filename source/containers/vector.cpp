// Copyright David Stone 2015.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <containers/vector.hpp>

#include "test_sequence_container.hpp"
#include "../test_int.hpp"

namespace {

static_assert(containers_test::test_sequence_container<containers::vector<bounded_test::integer>>());
static_assert(std::is_convertible_v<containers::vector<bounded_test::integer> const &, std::span<bounded_test::integer const>>);
static_assert(std::is_convertible_v<containers::vector<bounded_test::integer> &, std::span<bounded_test::integer>>);

} // namespace
