// Copyright David Stone 2015.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <compare>
#include <catch2/catch_test_macros.hpp>

import containers.static_vector;
import containers.test_sequence_container;
import containers.test_set_size;

import bounded;
import bounded.test_int;

using namespace bounded::literal;

template<typename T>
using test_static_vector = containers::static_vector<T, 40_bi>;

TEST_CASE("static_vector", "[static_vector]") {
	containers_test::test_sequence_container<test_static_vector<bounded_test::integer>>();
	containers_test::test_set_size<test_static_vector<bounded_test::integer>>();
}
