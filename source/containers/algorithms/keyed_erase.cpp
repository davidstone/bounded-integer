// Copyright David Stone 2021.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <containers/algorithms/keyed_erase.hpp>

#include <containers/flat_map.hpp>

#include "../../test_assert.hpp"
#include "../../test_int.hpp"

namespace {

using namespace bounded::literal;

using map_type = containers::flat_map<bounded_test::integer, bounded_test::integer>;
using multimap_type = containers::flat_multimap<bounded_test::integer, bounded_test::integer>;

template<typename Map>
constexpr auto test_empty() {
	auto m = Map();
	auto const erased = containers::keyed_erase(m, 0);
	BOUNDED_TEST(m == Map());
	BOUNDED_TEST(erased == 0_bi);
}

template<typename Map>
constexpr auto test_failed_erase_one() {
	auto m = Map({{1, 2}});
	auto const erased = containers::keyed_erase(m, 2);
	BOUNDED_TEST(m == Map({{1, 2}}));
	BOUNDED_TEST(erased == 0_bi);
}

template<typename Map>
constexpr auto test_successful_erase_one() {
	auto m = Map({{1, 2}});
	auto const erased = containers::keyed_erase(m, 1);
	BOUNDED_TEST(m == Map());
	BOUNDED_TEST(erased == 1_bi);
}

template<typename Map>
constexpr auto test_failed_erase_two() {
	auto m = Map({{1, 2}, {3, 4}});
	auto const erased = containers::keyed_erase(m, 2);
	BOUNDED_TEST(m == Map({{1, 2}, {3, 4}}));
	BOUNDED_TEST(erased == 0_bi);
}

template<typename Map>
constexpr auto test_successful_erase_two_first() {
	auto m = Map({{1, 2}, {3, 4}});
	auto const erased = containers::keyed_erase(m, 1);
	BOUNDED_TEST(m == Map({{3, 4}}));
	BOUNDED_TEST(erased == 1_bi);
}

template<typename Map>
constexpr auto test_successful_erase_two_second() {
	auto m = Map({{1, 2}, {3, 4}});
	auto const erased = containers::keyed_erase(m, 3);
	BOUNDED_TEST(m == Map({{1, 2}}));
	BOUNDED_TEST(erased == 1_bi);
}

template<typename Map>
constexpr auto test_successful_erase_middle() {
	auto m = Map({{1, 2}, {3, 4}, {5, 6}});
	auto const erased = containers::keyed_erase(m, 3);
	BOUNDED_TEST(m == Map({{1, 2}, {5, 6}}));
	BOUNDED_TEST(erased == 1_bi);
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
	BOUNDED_TEST(m == Map({{1, 2}, {1, 4}}));
	BOUNDED_TEST(erased == 0_bi);
	return true;
}

template<typename Map>
constexpr auto test_failed_erase_duplicates_before() {
	auto m = Map({{1, 2}, {1, 4}});
	auto const erased = containers::keyed_erase(m, 2);
	BOUNDED_TEST(m == Map({{1, 2}, {1, 4}}));
	BOUNDED_TEST(erased == 0_bi);
	return true;
}

template<typename Map>
constexpr auto test_successful_erase_duplicates_before() {
	auto m = Map({{1, 2}, {3, 4}, {3, 6}});
	auto const erased = containers::keyed_erase(m, 3);
	BOUNDED_TEST(m == Map({{1, 2}}));
	BOUNDED_TEST(erased == 2_bi);
	return true;
}

template<typename Map>
constexpr auto test_successful_erase_duplicates_middle() {
	auto m = Map({{1, 2}, {2, 4}, {2, 6}, {7, 8}});
	auto const erased = containers::keyed_erase(m, 2);
	BOUNDED_TEST(m == Map({{1, 2}, {7, 8}}));
	BOUNDED_TEST(erased == 2_bi);
	return true;
}

template<typename Map>
constexpr auto test_successful_erase_duplicates_after() {
	auto m = Map({{1, 2}, {1, 4}, {3, 6}});
	auto const erased = containers::keyed_erase(m, 1);
	BOUNDED_TEST(m == Map({{3, 6}}));
	BOUNDED_TEST(erased == 2_bi);
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

} // namespace