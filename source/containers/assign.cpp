// Copyright David Stone 2016.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <containers/assign.hpp>

#include <containers/algorithms/compare.hpp>
#include <containers/vector.hpp>

#include "../test_int.hpp"

namespace {

template<typename Container>
constexpr auto test_assign(auto const & source) -> bool {
	auto container = Container();
	containers::assign(container, source);
	BOUNDED_ASSERT(containers::equal(container, source));
	return true;
}

static_assert(test_assign<containers::vector<bounded_test::integer>>(containers::vector({bounded_test::integer(2)})));

} // namespace