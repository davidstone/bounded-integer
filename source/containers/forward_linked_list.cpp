// Copyright David Stone 2021.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <containers/forward_linked_list.hpp>

#include "test_sequence_container.hpp"
#include "../test_int.hpp"

namespace {

static_assert(bounded::convertible_to<containers::forward_linked_list<int>::iterator, containers::forward_linked_list<int>::const_iterator>);

static_assert(containers_test::test_sequence_container<containers::forward_linked_list<int>>());
static_assert(containers_test::test_sequence_container<containers::forward_linked_list<bounded_test::integer>>());

// TODO: Directly test `splice_after`, `erase_after`, and `lazy_insert_after`

struct recursive {
	containers::forward_linked_list<recursive> m;
};

} // namespace
