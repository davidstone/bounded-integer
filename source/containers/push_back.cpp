// Copyright David Stone 2016.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <containers/push_back.hpp>

#include <containers/vector.hpp>

#include "../test_int.hpp"

namespace {

static_assert([]{
	auto v = containers::vector<bounded_test::integer>();
	containers::push_back(v, 0);
	BOUNDED_TEST(v == containers::vector<bounded_test::integer>({0}));
	containers::push_back(v, 1);
	BOUNDED_TEST(v == containers::vector<bounded_test::integer>({0, 1}));
	containers::push_back(v, 2);
	BOUNDED_TEST(v == containers::vector<bounded_test::integer>({0, 1, 2}));
	containers::push_back(v, 3);
	BOUNDED_TEST(v == containers::vector<bounded_test::integer>({0, 1, 2, 3}));
	return true;
}());

} // namespace