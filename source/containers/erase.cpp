// Copyright David Stone 2020.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <containers/erase.hpp>

#include <containers/small_buffer_optimized_vector.hpp>
#include <containers/vector.hpp>

#include "../test_assert.hpp"

namespace {

template<typename Container>
constexpr auto test_erase() {
	Container v = {1, 2, 3, 4, 5, 6, 7};
	erase_if(v, [](auto const & value) { return value % 2 == 0; });
	BOUNDED_TEST(v == Container({1, 3, 5, 7}));
	return true;
}

static_assert(test_erase<containers::vector<int>>());
static_assert(test_erase<containers::small_buffer_optimized_vector<int, 0>>());

} // namespace