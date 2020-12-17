// Copyright David Stone 2020.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <containers/lookup.hpp>

#include <containers/flat_map.hpp>

#include "../test_assert.hpp"

namespace {

static_assert([]{
	auto const map = containers::flat_map<int, int>({{1, 3}});
	BOUNDED_TEST(*containers::lookup(map, 1) == 3);
	BOUNDED_TEST(!containers::lookup(map, 2));
	return true;
}());

} // namespace