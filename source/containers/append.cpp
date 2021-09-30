// Copyright David Stone 2016.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <containers/append.hpp>

#include <containers/vector.hpp>

#include "../test_int.hpp"

namespace {

using namespace bounded::literal;

using non_copyable = bounded_test::non_copyable_integer;

using container = containers::vector<bounded_test::non_copyable_integer>;

constexpr auto test_append(container const & expected, auto make_initial, auto... additional) {
	{
		auto c = make_initial();
		containers::append(c, {copy_value(additional)...});
		BOUNDED_ASSERT(c == expected);
	}
	{
		auto c = make_initial();
		containers::append(c, container({copy_value(additional)...}));
		BOUNDED_ASSERT(c == expected);
	}
	return true;
}

static_assert(test_append(
	container(),
	[] { return container(); }
));
static_assert(test_append(
	container({2}),
	[] { return container(); },
	non_copyable(2)
));
static_assert(test_append(
	container({1}),
	[] { return container({1}); }
));
static_assert(test_append(
	container({1, 2}),
	[] { return container({1}); },
	non_copyable(2)
));
static_assert(test_append(
	container({2}),
	[] {
		auto c = container();
		c.reserve(2_bi);
		return c;
	},
	non_copyable(2)
));
static_assert(test_append(
	container({1, 2}),
	[] {
		auto c = container({1});
		c.reserve(2_bi);
		return c;
	},
	non_copyable(2)
));

} // namespace