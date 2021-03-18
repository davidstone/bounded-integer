// Copyright David Stone 2020.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <containers/algorithms/erase.hpp>

#include <containers/vector.hpp>

#include "../../test_assert.hpp"
#include "../../test_int.hpp"

namespace {

using namespace bounded::literal;

template<typename Container>
constexpr auto test_erase_all(Container container) {
	containers::erase(container, containers::begin(container), containers::end(container));
	BOUNDED_TEST(container == Container());
}

constexpr auto test_erase_nothing_from_begin(auto container) {
	auto const original = container;
	auto it = containers::begin(container);
	containers::erase(container, it, it);
	BOUNDED_TEST(container == original);
}

constexpr auto test_erase_nothing_from_middle(auto container) {
	auto const original = container;
	auto it = containers::begin(container) + (containers::end(container) - containers::begin(container)) / 2_bi;
	containers::erase(container, it, it);
	BOUNDED_TEST(container == original);
}

constexpr auto test_erase_nothing_from_end(auto container) {
	auto const original = container;
	auto it = containers::end(container);
	containers::erase(container, it, it);
	BOUNDED_TEST(container == original);
}

constexpr auto test_erase_nothing(auto container) {
	test_erase_nothing_from_begin(container);
	test_erase_nothing_from_middle(container);
	test_erase_nothing_from_end(container);
}


template<typename Container>
constexpr auto test_erase_empty() {
	test_erase_all(Container());
}


template<typename Container>
constexpr auto test_erase_nothing_from_one() {
	test_erase_nothing(Container({1}));
}

template<typename Container>
constexpr auto test_erase_all_from_one() {
	test_erase_all(Container({1}));
}

template<typename Container>
constexpr auto test_erase_from_one() {
	auto v = Container({1});
	containers::erase(v, containers::begin(v));
	BOUNDED_TEST(v == Container({}));
}


template<typename Container>
constexpr auto test_erase_nothing_from_two() {
	test_erase_nothing(Container({1, 2}));
}

template<typename Container>
constexpr auto test_erase_all_from_two() {
	test_erase_all(Container({1, 2}));
}

template<typename Container>
constexpr auto test_erase_first_from_two() {
	auto v = Container({1, 2});
	containers::erase(v, containers::begin(v));
	BOUNDED_TEST(v == Container({2}));
}

template<typename Container>
constexpr auto test_erase_second_from_two() {
	auto v = Container({1, 2});
	containers::erase(v, containers::begin(v) + 1_bi);
	BOUNDED_TEST(v == Container({1}));
}

template<typename Container>
constexpr auto test_erase_middle_range() {
	auto v = Container({1, 2, 3, 4});
	containers::erase(v, containers::begin(v) + 1_bi, containers::begin(v) + 3_bi);
	BOUNDED_TEST(v == Container({1, 4}));
}


template<typename Container>
constexpr auto test_erase_if() {
	auto v = Container({1, 2, 3, 4, 5, 6, 7});
	erase_if(v, [](auto const & x) { return x.value() % 2 == 0; });
	BOUNDED_TEST(v == Container({1, 3, 5, 7}));
}

template<typename Container>
constexpr auto test_all() {
	test_erase_empty<Container>();
	test_erase_nothing_from_one<Container>();
	test_erase_all_from_one<Container>();
	test_erase_from_one<Container>();
	test_erase_nothing_from_two<Container>();
	test_erase_all_from_two<Container>();
	test_erase_first_from_two<Container>();
	test_erase_second_from_two<Container>();
	test_erase_middle_range<Container>();
	test_erase_if<Container>();
	return true;
}

static_assert(test_all<containers::vector<bounded::test_int>>());

} // namespace