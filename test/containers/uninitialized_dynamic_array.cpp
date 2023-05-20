// Copyright David Stone 2023.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

module;

#include <bounded/assert.hpp>

export module containers.test.uninitialized_dynamic_array;

import containers.test.test_sequence_container;

import containers.uninitialized_dynamic_array;

import bounded;
import bounded.test_int;
import std_module;

using namespace bounded::literal;

template<int max>
using container = containers::uninitialized_dynamic_array<bounded_test::integer, bounded::integer<0, max>>;

static_assert(container<0>().capacity() == 0_bi);
static_assert(container<1>().capacity() == 0_bi);

constexpr auto basic_test() -> bool {
	auto a = container<10>(1_bi);
	auto b = container<10>(1_bi);
	BOUNDED_ASSERT(a.data() != nullptr);
	BOUNDED_ASSERT(b.data() != nullptr);
	BOUNDED_ASSERT(a.data() != b.data());
	BOUNDED_ASSERT(a.capacity() == 1_bi);
	BOUNDED_ASSERT(b.capacity() == 1_bi);
	return true;
}
static_assert(basic_test());

constexpr auto move_construction() -> bool {
	auto a = container<1>(1_bi);
	auto const original_pointer = a.data();
	auto b = std::move(a);
	BOUNDED_ASSERT(a.data() == nullptr);
	BOUNDED_ASSERT(b.data() == original_pointer);
	BOUNDED_ASSERT(b.capacity() == 1_bi);
	return true;
}
static_assert(move_construction());

constexpr auto check_capacity(auto const value) {
	return [=](auto const & container) {
		return container.capacity() == value;
	};
}


static_assert(containers_test::test_self_move_assignment<container<10>>(
	[] { return 0_bi; },
	check_capacity(0_bi)
));
static_assert(containers_test::test_self_move_assignment<container<10>>(
	[] { return 1_bi; },
	check_capacity(1_bi)
));
static_assert(containers_test::test_self_move_assignment<container<0>>(
	[] { return 0_bi; },
	check_capacity(0_bi)
));

static_assert(containers_test::test_self_swap(container<10>(0_bi), check_capacity(0_bi)));
static_assert(containers_test::test_self_swap(container<10>(1_bi), check_capacity(1_bi)));
static_assert(containers_test::test_self_swap(container<0>(0_bi), check_capacity(0_bi)));
