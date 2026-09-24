// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

module;

#include <bounded/assert.hpp>

export module containers.test.flat_set;

import containers.algorithms.compare;

import containers.test.test_reserve_and_capacity;
import containers.test.test_sequence_container;

import containers.associative_container;
import containers.array;
import containers.flat_set;
import containers.is_container;
import containers.map_tags;
import containers.maximum_array_size;
import containers.range_size_t;
import containers.string_view;

import bounded;
import bounded.test_int;
import numeric_traits;
import std_module;

using set = containers::flat_set<bounded_test::non_copyable_integer>;

static_assert(containers_test::test_sequence_container<set>());

template<typename Key>
constexpr auto max_set_size = numeric_traits::max_value<containers::range_size_t<containers::flat_set<Key>>>;

using namespace bounded::literal;
static_assert(max_set_size<bounded::integer<0, 2>> == 3_bi);
static_assert(max_set_size<bounded::integer<1, 2>> == 2_bi);
static_assert(max_set_size<bool> == 2_bi);
static_assert(max_set_size<std::byte> == 256_bi);
static_assert(max_set_size<containers::string_view> == containers::maximum_array_size<containers::string_view>);

static_assert(containers::associative_range<set &>);
static_assert(containers::associative_range<set const &>);
static_assert(containers::associative_range<set &&>);
static_assert(containers::associative_range<set>);
static_assert(!containers::associative_container<set &>);
static_assert(!containers::associative_container<set const &>);
static_assert(containers::associative_container<set &&>);
static_assert(containers::associative_container<set>);

constexpr auto array(auto const ... values) {
	using result = containers::array<bounded_test::non_copyable_integer, bounded::constant<sizeof...(values)>>;
	return result{values...};
}

static_assert(containers_test::test_reserve_and_capacity<set>());

static_assert(containers::equal(set(), array()));

static_assert(containers::equal(set({}), array()));
static_assert(containers::equal(set(containers::assume_unique, {}), array()));
static_assert(containers::equal(set(containers::assume_sorted_unique, {}), array()));

static_assert(containers::equal(set({1}), array(1)));
static_assert(containers::equal(set(containers::assume_unique, {1}), array(1)));
static_assert(containers::equal(set(containers::assume_sorted_unique, {1}), array(1)));

static_assert(containers::equal(set({1, 2}), array(1, 2)));
static_assert(containers::equal(set(containers::assume_unique, {1, 2}), array(1, 2)));
static_assert(containers::equal(set(containers::assume_sorted_unique, {1, 2}), array(1, 2)));
static_assert(containers::equal(set({2, 1}), array(1, 2)));
static_assert(containers::equal(set(containers::assume_unique, {2, 1}), array(1, 2)));
static_assert(containers::equal(set({1, 1}), array(1)));

static_assert(containers::equal(set({1, 2, 3}), array(1, 2, 3)));
static_assert(containers::equal(set(containers::assume_unique, {1, 2, 3}), array(1, 2, 3)));
static_assert(containers::equal(set(containers::assume_sorted_unique, {1, 2, 3}), array(1, 2, 3)));
static_assert(containers::equal(set({1, 3, 2}), array(1, 2, 3)));
static_assert(containers::equal(set(containers::assume_unique, {1, 3, 2}), array(1, 2, 3)));
static_assert(containers::equal(set({2, 1, 3}), array(1, 2, 3)));
static_assert(containers::equal(set(containers::assume_unique, {2, 1, 3}), array(1, 2, 3)));
static_assert(containers::equal(set({2, 3, 1}), array(1, 2, 3)));
static_assert(containers::equal(set(containers::assume_unique, {2, 3, 1}), array(1, 2, 3)));
static_assert(containers::equal(set({3, 1, 2}), array(1, 2, 3)));
static_assert(containers::equal(set(containers::assume_unique, {3, 1, 2}), array(1, 2, 3)));
static_assert(containers::equal(set({3, 2, 1}), array(1, 2, 3)));
static_assert(containers::equal(set(containers::assume_unique, {3, 2, 1}), array(1, 2, 3)));
static_assert(containers::equal(set({1, 1, 2}), array(1, 2)));
static_assert(containers::equal(set({1, 2, 1}), array(1, 2)));
static_assert(containers::equal(set({2, 1, 1}), array(1, 2)));
static_assert(containers::equal(set({1, 2, 2}), array(1, 2)));
static_assert(containers::equal(set({2, 1, 2}), array(1, 2)));
static_assert(containers::equal(set({2, 2, 1}), array(1, 2)));
static_assert(containers::equal(set({1, 1, 1}), array(1)));

constexpr auto inserted(set s, int const value) -> set {
	s.insert(value);
	return s;
}

static_assert(inserted(set(), 1) == set({1}));
static_assert(inserted(set({1}), 1) == set({1}));
static_assert(inserted(set({1}), 2) == set({1, 2}));
static_assert(inserted(set({2}), 1) == set({1, 2}));
static_assert(inserted(set({1, 2}), 1) == set({1, 2}));
static_assert(inserted(set({1, 2}), 2) == set({1, 2}));
static_assert(inserted(set({1, 2}), 3) == set({1, 2, 3}));
