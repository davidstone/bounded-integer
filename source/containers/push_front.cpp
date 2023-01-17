// Copyright David Stone 2022.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

module;

#include "../test_assert.hpp"

export module containers.push_front;

import containers.bidirectional_linked_list;
import containers.forward_linked_list;
import containers.front_back;
import containers.lazy_push_front;
import containers.range_value_t;

import bounded;
import bounded.test_int;
import std_module;

namespace containers {

template<typename Container>
concept member_push_frontable = requires(Container & container, range_value_t<Container> value) {
	container.push_front(std::move(value));
};

template<typename Container>
concept push_frontable = member_push_frontable<Container> or lazy_push_frontable<Container>;

export template<push_frontable Container> requires std::copy_constructible<range_value_t<Container>>
constexpr auto push_front(Container & container, range_value_t<Container> const & value) -> auto & {
	if constexpr (member_push_frontable<Container>) {
		container.push_front(value);
		return containers::front(container);
	} else {
		return ::containers::lazy_push_front(
			container,
			::bounded::value_to_function(value)
		);
	}
}
export template<push_frontable Container> requires std::move_constructible<range_value_t<Container>>
constexpr auto push_front(Container & container, range_value_t<Container> && value) -> auto & {
	if constexpr (member_push_frontable<Container>) {
		container.push_front(std::move(value));
		return containers::front(container);
	} else {
		return ::containers::lazy_push_front(
			container,
			::bounded::value_to_function(std::move(value))
		);
	}
}

} // namespace containers

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
