// Copyright David Stone 2021.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

module;

#include "../test_assert.hpp"

export module containers.assign_to_empty_test;

import containers.assign_to_empty;
import containers.data;
import containers.is_empty;
import containers.vector;

import bounded;
import bounded.test_int;

using namespace bounded::literal;

using non_copyable = bounded_test::non_copyable_integer;

using container = containers::vector<non_copyable>;

constexpr auto test_assign(bool const same_allocation, auto make_initial, auto... additional) {
	auto individual_test = [&](auto do_assign) {
		auto c = make_initial();
		BOUNDED_TEST(containers::is_empty(c));
		auto const ptr = containers::data(c);
		do_assign(c);
		BOUNDED_TEST(c == container({copy_value(additional)...}));
		if (same_allocation) {
			BOUNDED_TEST(ptr == containers::data(c));
		}
	};
	individual_test([&](container & c) { containers::assign_to_empty(c, {copy_value(additional)...}); });
	individual_test([&](container & c) { containers::assign_to_empty(c, container({copy_value(additional)...})); });
	return true;
}

static_assert(test_assign(
	true,
	[] { return container(); }
));
static_assert(test_assign(
	false,
	[] { return container(); },
	non_copyable(2)
));
static_assert(test_assign(
	true,
	[] {
		auto c = container();
		c.reserve(2_bi);
		return c;
	},
	non_copyable(2)
));