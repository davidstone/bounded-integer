// Copyright David Stone 2021.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <containers/bidirectional_linked_list.hpp>

#include "test_sequence_container.hpp"
#include "../test_int.hpp"

namespace {

static_assert(std::is_convertible_v<containers::bidirectional_linked_list<int>::iterator, containers::bidirectional_linked_list<int>::const_iterator>);

static_assert(containers_test::test_sequence_container<containers::bidirectional_linked_list<int>>());
static_assert(containers_test::test_sequence_container<containers::bidirectional_linked_list<bounded_test::integer>>());

// TODO: Directly test `splice`, `pop_back`, and `lazy_push_back`

struct recursive {
	containers::bidirectional_linked_list<recursive> m;
};

} // namespace
