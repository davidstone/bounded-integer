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

constexpr auto add = []<typename T>(T & lhs, auto const & rhs) {
	lhs = T(lhs.value() + rhs.value());
};

template<typename Container>
constexpr auto test_upsert_empty_empty() -> void {
	auto container = Container();
	container.upsert(Container(), add);
	BOUNDED_TEST(container == Container());
}

template<typename Container>
constexpr auto test_upsert_non_empty_empty() -> void {
	constexpr auto make = [] { return Container(containers_test::make_one_key<Container>()); };
	auto container = make();
	container.upsert(Container(), add);
	BOUNDED_TEST(container == make());
}

template<typename Container>
constexpr auto test_upsert_empty_non_empty() -> void {
	constexpr auto make = [] { return Container(containers_test::make_one_key<Container>()); };
	auto container = Container();
	container.upsert(make(), add);
	BOUNDED_TEST(container == make());
}

template<typename Container>
constexpr auto test_upsert_1_2() -> void {
	auto container = Container({{1, 3}});
	container.upsert(Container({{2, 4}}), add);
	BOUNDED_TEST(container == Container({{1, 3}, {2, 4}}));
}

template<typename Container>
constexpr auto test_upsert_2_1() -> void {
	auto container = Container({{2, 4}});
	container.upsert(Container({{1, 3}}), add);
	BOUNDED_TEST(container == Container({{1, 3}, {2, 4}}));
}

template<typename Container>
constexpr auto test_upsert_1_1() -> void {
	auto container = Container({{1, 4}});
	container.upsert(Container({{1, 3}}), add);
	BOUNDED_TEST(container == Container({{1, 7}}));
}

template<typename Container>
constexpr auto test_upsert() -> bool {
	test_upsert_empty_empty<Container>();
	test_upsert_non_empty_empty<Container>();
	test_upsert_empty_non_empty<Container>();
	test_upsert_1_2<Container>();
	test_upsert_2_1<Container>();
	test_upsert_1_1<Container>();
	return true;
}
static_assert(test_upsert<non_copyable_map>());

} // namespace
