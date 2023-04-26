// Copyright David Stone 2021.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

module;

#include <bounded/assert.hpp>

#include <operators/forward.hpp>

export module containers.algorithms.keyed_insert;

import containers.associative_container;
import containers.flat_map;
import containers.map_value_type;
import containers.range_value_t;

import bounded;
import bounded.test_int;
import std_module;

namespace containers {

template<typename Map>
concept legacy_map = requires { typename Map::allocator_type; };

export template<
	associative_container Map,
	bounded::convertible_to<typename Map::key_type> Key = typename Map::key_type,
	bounded::convertible_to<typename Map::mapped_type> Mapped = typename Map::mapped_type
>
constexpr auto keyed_insert(Map & map, Key && key, Mapped && mapped) {
	if constexpr (legacy_map<Map>) {
		return map.insert({OPERATORS_FORWARD(key), OPERATORS_FORWARD(mapped)});
	} else {
		return map.lazy_insert(OPERATORS_FORWARD(key), bounded::value_to_function(OPERATORS_FORWARD(mapped)));
	}
}
export template<associative_container Map>
constexpr auto keyed_insert(Map & map, range_value_t<Map> const & value) {
	if constexpr (legacy_map<Map>) {
		return map.insert(value);
	} else {
		return map.lazy_insert(get_key(value), [&] -> decltype(auto) { return get_mapped(value); });
	}
}
export template<associative_container Map>
constexpr auto keyed_insert(Map & map, range_value_t<Map> && value) {
	if constexpr (legacy_map<Map>) {
		return map.insert(std::move(value));
	} else {
		return map.lazy_insert(get_key(std::move(value)), [&] -> decltype(auto) { return get_mapped(std::move(value)); });
	}
}

} // namespace containers

using map_type = containers::flat_map<bounded_test::integer, bounded_test::integer>;
using multimap_type = containers::flat_map<bounded_test::integer, bounded_test::integer>;
using value_type = containers::range_value_t<map_type>;

template<typename Map>
constexpr auto test_rvalue_into_empty(auto function) {
	auto m = Map();
	function(m, 1, 2);
	BOUNDED_ASSERT(m == Map({{1, 2}}));
}

template<typename Map>
constexpr auto test_lvalue_into_empty() {
	auto m = Map();
	auto value = value_type{1, 2};
	containers::keyed_insert(m, value);
	BOUNDED_ASSERT(m == Map({{1, 2}}));
}

template<typename Map>
constexpr auto test_before_value(auto function) {
	auto m = Map({{3, 4}});
	function(m, 1, 2);
	BOUNDED_ASSERT(m == Map({{1, 2}, {3, 4}}));
}

template<typename Map>
constexpr auto test_after_value(auto function) {
	auto m = Map({{1, 2}});
	function(m, 3, 4);
	BOUNDED_ASSERT(m == Map({{1, 2}, {3, 4}}));
}

template<typename Map>
constexpr auto test_between_values(auto function) {
	auto m = Map({{1, 2}, {5, 6}});
	function(m, 3, 4);
	BOUNDED_ASSERT(m == Map({{1, 2}, {3, 4}, {5, 6}}));
}

constexpr auto one_argument = [](auto & container, auto && key, auto && mapped) {
	containers::keyed_insert(container, {OPERATORS_FORWARD(key), OPERATORS_FORWARD(mapped)});
};
constexpr auto two_argument = [](auto & container, auto && key, auto && mapped) {
	containers::keyed_insert(container, OPERATORS_FORWARD(key), OPERATORS_FORWARD(mapped));
};

template<typename Map>
constexpr auto test_all(auto function) {
	test_rvalue_into_empty<Map>(function);
	test_lvalue_into_empty<Map>();
	test_before_value<Map>(function);
	test_after_value<Map>(function);
	test_between_values<Map>(function);
	return true;
}

template<typename Map>
constexpr auto test_duplicate(auto function) {
	auto m = Map({{1, 2}});
	function(m, 1, 4);
	BOUNDED_ASSERT(m == Map({{1, 2}}));
	return true;
}

static_assert(test_all<map_type>(one_argument));
static_assert(test_all<map_type>(two_argument));
static_assert(test_all<multimap_type>(one_argument));
static_assert(test_all<multimap_type>(two_argument));
static_assert(test_duplicate<multimap_type>(one_argument));
static_assert(test_duplicate<multimap_type>(two_argument));