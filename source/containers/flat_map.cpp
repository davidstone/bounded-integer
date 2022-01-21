// Copyright David Stone 2015.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <containers/flat_map.hpp>

#include "test_associative_container.hpp"
#include "test_reserve_and_capacity.hpp"
#include "../test_int.hpp"

namespace {

using non_copyable_map = containers::flat_map<bounded_test::non_copyable_integer, bounded_test::non_copyable_integer>;

static_assert(containers_test::test_reserve_and_capacity<non_copyable_map>());
static_assert(containers_test::test_associative_container<non_copyable_map>());

template<typename Container>
constexpr auto test_zero_keys() -> void {
	constexpr auto make = [] { return containers_test::make_zero_keys<Container>(); };
	static_assert(containers::equal(
		Container(containers::assume_sorted_unique, make()),
		make()
	));
}

template<typename Container>
constexpr auto test_one_key() -> void {
	constexpr auto make = [] { return containers_test::make_one_key<Container>(); };
	static_assert(containers::equal(
		Container(containers::assume_sorted_unique, make()),
		make()
	));
}

template<typename Container>
constexpr auto test_two_sorted_unique_keys() -> void {
	constexpr auto make = [] { return containers_test::make_two_sorted_unique_keys<Container>(); };
	static_assert(containers::equal(
		Container(containers::assume_sorted_unique, make()),
		make()
	));
}

template<typename Container>
constexpr auto test_three_sorted_unique_keys() -> void {
	constexpr auto make = [] { return containers_test::make_three_sorted_unique_keys<Container>(); };
	static_assert(containers::equal(
		Container(containers::assume_sorted_unique, make()),
		make()
	));
}

template<typename Container>
constexpr auto test_assume_sorted_unique() -> bool {
	test_zero_keys<Container>();
	test_one_key<Container>();
	test_two_sorted_unique_keys<Container>();
	test_three_sorted_unique_keys<Container>();
	return true;
}
static_assert(test_assume_sorted_unique<non_copyable_map>());

} // namespace
