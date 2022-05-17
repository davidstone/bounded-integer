// Copyright David Stone 2022.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <containers/push_front.hpp>

#include <containers/bidirectional_linked_list.hpp>
#include <containers/forward_linked_list.hpp>

#include "../test_int.hpp"

namespace {

template<typename Container>
constexpr auto test_push_front() -> bool {
	auto v = Container();
	containers::push_front(v, 0);
	BOUNDED_TEST(v == Container({0}));
	containers::push_front(v, 1);
	BOUNDED_TEST(v == Container({1, 0}));
	containers::push_front(v, 2);
	BOUNDED_TEST(v == Container({2, 1, 0}));
	containers::push_front(v, 3);
	BOUNDED_TEST(v == Container({3, 2, 1, 0}));
	return true;
}

static_assert(test_push_front<containers::bidirectional_linked_list<bounded_test::non_copyable_integer>>());
static_assert(test_push_front<containers::forward_linked_list<bounded_test::non_copyable_integer>>());

} // namespace