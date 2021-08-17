// Copyright David Stone 2015.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <containers/stable_vector.hpp>

#include <containers/index_type.hpp>

#include <numeric_traits/min_max_value.hpp>

#include "test_append_from_capacity.hpp"
#include "test_sequence_container.hpp"

#include "../test_int.hpp"

namespace {

using namespace bounded::literal;

template<typename T>
using test_stable_vector = containers::stable_vector<T, 1000>;

static_assert(test_stable_vector<int>::capacity() == 1000_bi);

static_assert(containers_test::test_sequence_container<test_stable_vector<int>>());
static_assert(containers_test::test_sequence_container<test_stable_vector<bounded_test::integer>>());

static_assert(containers_test::test_append_from_capacity<test_stable_vector<int>>());
static_assert(containers_test::test_append_from_capacity<test_stable_vector<bounded_test::integer>>());

using index_type = containers::index_type<containers::stable_vector<int, 10>>;
static_assert(numeric_traits::min_value<index_type> == 0_bi);
static_assert(numeric_traits::max_value<index_type> == 9_bi);

struct recursive {
	containers::stable_vector<recursive, 1> m;
};

} // namespace
