// Copyright David Stone 2021.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

module;

#include <bounded/assert.hpp>

#include <operators/forward.hpp>

export module containers.algorithms.keyed_erase;

import containers.algorithms.erase;
import containers.algorithms.keyed_binary_search;
import containers.associative_container;
import containers.begin_end;
import containers.flat_map;
import containers.size;

import bounded;
import bounded.test_int;

using namespace bounded::literal;

namespace containers {

export constexpr auto keyed_erase(associative_container auto & map, auto && key) {
	if constexpr (requires { map.find(OPERATORS_FORWARD(key)); }) {
		auto const it = map.find(OPERATORS_FORWARD(key));
		if (it == containers::end(map)) {
			return bounded::integer<0, 1>(bounded::constant<0>);
		}
		map.erase(it);
		return bounded::integer<0, 1>(bounded::constant<1>);
	} else {
		// TODO: This can be made more efficient for node-based containers
		auto const to_erase = keyed_equal_range(map, OPERATORS_FORWARD(key));
		auto const erased = containers::size(to_erase);
		containers::erase(map, containers::begin(to_erase), containers::end(to_erase));
		return erased;
	}
}

} // namespace containers

using map_type = containers::flat_map<bounded_test::integer, bounded_test::integer>;
using multimap_type = containers::flat_multimap<bounded_test::integer, bounded_test::integer>;

template<typename Map>
constexpr auto test_empty() {
	auto m = Map();
	auto const erased = containers::keyed_erase(m, 0);
	BOUNDED_ASSERT(m == Map());
	BOUNDED_ASSERT(erased == 0_bi);
}

template<typename Map>
constexpr auto test_failed_erase_one() {
	auto m = Map({{1, 2}});
	auto const erased = containers::keyed_erase(m, 2);
	BOUNDED_ASSERT(m == Map({{1, 2}}));
	BOUNDED_ASSERT(erased == 0_bi);
}

template<typename Map>
constexpr auto test_successful_erase_one() {
	auto m = Map({{1, 2}});
	auto const erased = containers::keyed_erase(m, 1);
	BOUNDED_ASSERT(m == Map());
	BOUNDED_ASSERT(erased == 1_bi);
}

template<typename Map>
constexpr auto test_failed_erase_two() {
	auto m = Map({{1, 2}, {3, 4}});
	auto const erased = containers::keyed_erase(m, 2);
	BOUNDED_ASSERT(m == Map({{1, 2}, {3, 4}}));
	BOUNDED_ASSERT(erased == 0_bi);
}

template<typename Map>
constexpr auto test_successful_erase_two_first() {
	auto m = Map({{1, 2}, {3, 4}});
	auto const erased = containers::keyed_erase(m, 1);
	BOUNDED_ASSERT(m == Map({{3, 4}}));
	BOUNDED_ASSERT(erased == 1_bi);
}

template<typename Map>
constexpr auto test_successful_erase_two_second() {
	auto m = Map({{1, 2}, {3, 4}});
	auto const erased = containers::keyed_erase(m, 3);
	BOUNDED_ASSERT(m == Map({{1, 2}}));
	BOUNDED_ASSERT(erased == 1_bi);
}

template<typename Map>
constexpr auto test_successful_erase_middle() {
	auto m = Map({{1, 2}, {3, 4}, {5, 6}});
	auto const erased = containers::keyed_erase(m, 3);
	BOUNDED_ASSERT(m == Map({{1, 2}, {5, 6}}));
	BOUNDED_ASSERT(erased == 1_bi);
}

template<typename Map>
constexpr auto test_all() {
	test_empty<Map>();
	test_failed_erase_one<Map>();
	test_successful_erase_one<Map>();
	test_failed_erase_two<Map>();
	test_successful_erase_two_first<Map>();
	test_successful_erase_two_second<Map>();
	test_successful_erase_middle<Map>();
	return true;
}

template<typename Map>
constexpr auto test_failed_erase_duplicates_after() {
	auto m = Map({{1, 2}, {1, 4}});
	auto const erased = containers::keyed_erase(m, 0);
	BOUNDED_ASSERT(m == Map({{1, 2}, {1, 4}}));
	BOUNDED_ASSERT(erased == 0_bi);
	return true;
}

template<typename Map>
constexpr auto test_failed_erase_duplicates_before() {
	auto m = Map({{1, 2}, {1, 4}});
	auto const erased = containers::keyed_erase(m, 2);
	BOUNDED_ASSERT(m == Map({{1, 2}, {1, 4}}));
	BOUNDED_ASSERT(erased == 0_bi);
	return true;
}

template<typename Map>
constexpr auto test_successful_erase_duplicates_before() {
	auto m = Map({{1, 2}, {3, 4}, {3, 6}});
	auto const erased = containers::keyed_erase(m, 3);
	BOUNDED_ASSERT(m == Map({{1, 2}}));
	BOUNDED_ASSERT(erased == 2_bi);
	return true;
}

template<typename Map>
constexpr auto test_successful_erase_duplicates_middle() {
	auto m = Map({{1, 2}, {2, 4}, {2, 6}, {7, 8}});
	auto const erased = containers::keyed_erase(m, 2);
	BOUNDED_ASSERT(m == Map({{1, 2}, {7, 8}}));
	BOUNDED_ASSERT(erased == 2_bi);
	return true;
}

template<typename Map>
constexpr auto test_successful_erase_duplicates_after() {
	auto m = Map({{1, 2}, {1, 4}, {3, 6}});
	auto const erased = containers::keyed_erase(m, 1);
	BOUNDED_ASSERT(m == Map({{3, 6}}));
	BOUNDED_ASSERT(erased == 2_bi);
	return true;
}

template<typename Map>
constexpr auto test_duplicates() {
	test_failed_erase_duplicates_after<Map>();
	test_failed_erase_duplicates_before<Map>();
	test_successful_erase_duplicates_before<Map>();
	test_successful_erase_duplicates_middle<Map>();
	test_successful_erase_duplicates_after<Map>();
	return true;
}

static_assert(test_all<map_type>());
static_assert(test_all<multimap_type>());
static_assert(test_duplicates<multimap_type>());