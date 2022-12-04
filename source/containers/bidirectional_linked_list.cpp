// Copyright David Stone 2021.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <containers/bidirectional_linked_list.hpp>

#include "test_sequence_container.hpp"
#include "../test_int.hpp"

namespace {

static_assert(bounded::convertible_to<containers::bidirectional_linked_list<int>::iterator, containers::bidirectional_linked_list<int>::const_iterator>);

static_assert(containers_test::test_sequence_container<containers::bidirectional_linked_list<int>>());
static_assert(containers_test::test_sequence_container<containers::bidirectional_linked_list<bounded_test::integer>>());

template<typename Integer>
constexpr auto test_lazy_push_back_empty() {
	auto list = containers::bidirectional_linked_list<Integer>();
	auto const & element = list.lazy_push_back(bounded::construct<Integer>);
	BOUNDED_TEST(containers::equal(list, containers::array{0}));
	BOUNDED_TEST(std::addressof(element) == std::addressof(containers::front(list)));
}

template<typename Integer>
constexpr auto test_lazy_push_back_non_empty() {
	auto list = containers::bidirectional_linked_list<Integer>({1});
	auto const ptr = std::addressof(containers::front(list));
	auto const & element = list.lazy_push_back(bounded::construct<Integer>);
	BOUNDED_TEST(containers::equal(list, containers::array{1, 0}));
	BOUNDED_TEST(std::addressof(element) == std::addressof(containers::back(list)));
	BOUNDED_TEST(ptr == std::addressof(containers::front(list)));
}

template<typename Integer>
constexpr auto test_lazy_push_back() {
	test_lazy_push_back_empty<Integer>();
	test_lazy_push_back_non_empty<Integer>();
	return true;
}

static_assert(test_lazy_push_back<int>());
static_assert(test_lazy_push_back<bounded_test::non_copyable_integer>());

// TODO: Directly test `splice`, `pop_back`

struct recursive {
	containers::bidirectional_linked_list<recursive> m;
};

} // namespace
