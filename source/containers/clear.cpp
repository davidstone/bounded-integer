// Copyright David Stone 2016.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <containers/clear.hpp>

#include <containers/is_empty.hpp>
#include <containers/vector.hpp>

#include "../test_assert.hpp"
#include "../test_int.hpp"

namespace {

static_assert([]{
	auto v = containers::vector<bounded_test::non_copyable_integer>({1});
	containers::clear(v);
	BOUNDED_TEST(containers::is_empty(v));
	return true;
}());

static_assert([]{
	auto v = containers::vector<bounded_test::non_copyable_integer>();
	containers::clear(v);
	BOUNDED_TEST(containers::is_empty(v));
	return true;
}());

} // namespace