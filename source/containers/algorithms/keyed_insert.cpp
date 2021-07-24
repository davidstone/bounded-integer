// Copyright David Stone 2021.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <containers/algorithms/keyed_insert.hpp>

#include <containers/flat_map.hpp>
#include <containers/range_value_t.hpp>

#include "../../test_assert.hpp"
#include "../../test_int.hpp"

namespace {

using map_type = containers::flat_map<bounded_test::integer, bounded_test::integer>;
using multimap_type = containers::flat_map<bounded_test::integer, bounded_test::integer>;
using value_type = containers::range_value_t<map_type>;

template<typename Map>
constexpr auto test_rvalue_into_empty() {
	auto m = Map();
	containers::keyed_insert(m, {1, 2});
	BOUNDED_TEST(m == Map({{1, 2}}));
}

template<typename Map>
constexpr auto test_lvalue_into_empty() {
	auto m = Map();
	auto value = value_type{1, 2};
	containers::keyed_insert(m, value);
	BOUNDED_TEST(m == Map({{1, 2}}));
}

template<typename Map>
constexpr auto test_before_value() {
	auto m = Map({{3, 4}});
	containers::keyed_insert(m, {1, 2});
	BOUNDED_TEST(m == Map({{1, 2}, {3, 4}}));
}

template<typename Map>
constexpr auto test_after_value() {
	auto m = Map({{1, 2}});
	containers::keyed_insert(m, {3, 4});
	BOUNDED_TEST(m == Map({{1, 2}, {3, 4}}));
}

template<typename Map>
constexpr auto test_between_values() {
	auto m = Map({{1, 2}, {5, 6}});
	containers::keyed_insert(m, {3, 4});
	BOUNDED_TEST(m == Map({{1, 2}, {3, 4}, {5, 6}}));
}

template<typename Map>
constexpr auto test_all() {
	test_rvalue_into_empty<Map>();
	test_lvalue_into_empty<Map>();
	test_before_value<Map>();
	test_after_value<Map>();
	test_between_values<Map>();
	return true;
}

template<typename Map>
constexpr auto test_duplicate() {
	auto m = Map({{1, 2}});
	containers::keyed_insert(m, {1, 4});
	BOUNDED_TEST(m == Map({{1, 2}}));
	return true;
}

static_assert(test_all<map_type>());
static_assert(test_all<multimap_type>());
static_assert(test_duplicate<multimap_type>());

} // namespace