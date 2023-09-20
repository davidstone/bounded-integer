// Copyright David Stone 2023.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

export module containers.test.stable_vector;

import containers.test.test_sequence_container;
import containers.test.test_set_size;

import containers.index_type;
import containers.stable_vector;

import bounded;
import bounded.test_int;
import numeric_traits;

using namespace bounded::literal;

template<typename T>
using test_stable_vector = containers::stable_vector<T, 1000_bi>;

static_assert(test_stable_vector<int>::capacity() == 1000_bi);

static_assert(containers_test::test_sequence_container<test_stable_vector<int>>());
static_assert(containers_test::test_sequence_container<test_stable_vector<bounded_test::integer>>());

static_assert(containers_test::test_set_size<test_stable_vector<int>>());
static_assert(containers_test::test_set_size<test_stable_vector<bounded_test::integer>>());

using index_type = containers::index_type<containers::stable_vector<int, 10_bi>>;
static_assert(numeric_traits::min_value<index_type> == 0_bi);
static_assert(numeric_traits::max_value<index_type> == 9_bi);

namespace {

struct recursive {
	containers::stable_vector<recursive, 1_bi> m;
};

} // namespace