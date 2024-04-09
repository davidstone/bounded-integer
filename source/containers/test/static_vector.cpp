// Copyright David Stone 2015.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <std_module/prelude.hpp>
#include <catch2/catch_test_macros.hpp>

import containers.test.test_sequence_container;
import containers.test.test_set_size;

import containers.array;
import containers.c_array;
import containers.index_type;
import containers.iterator;
import containers.static_vector;

import bounded;
import bounded.homogeneous_equals;
import bounded.test_int;
import numeric_traits;

using namespace bounded::literal;

struct non_trivial {
	constexpr non_trivial(non_trivial &&) noexcept {
	}
	constexpr non_trivial(non_trivial const &) {
	}
	constexpr ~non_trivial() noexcept {
	}
	constexpr auto operator=(non_trivial &&) noexcept -> non_trivial & {
		return *this;
	}
	constexpr auto operator=(non_trivial const &) -> non_trivial & {
		return *this;
	}
};

static_assert(bounded::trivially_move_constructible<containers::static_vector<int, 0_bi>>);
static_assert(bounded::trivially_move_constructible<containers::static_vector<int, 1_bi>>);
static_assert(bounded::trivially_move_constructible<containers::static_vector<non_trivial, 0_bi>>);
static_assert(!bounded::trivially_move_constructible<containers::static_vector<non_trivial, 1_bi>>);
static_assert(bounded::move_constructible<containers::static_vector<non_trivial, 1_bi>>);

static_assert(bounded::trivially_copy_constructible<containers::static_vector<int, 0_bi>>);
static_assert(bounded::trivially_copy_constructible<containers::static_vector<int, 1_bi>>);
static_assert(bounded::trivially_copy_constructible<containers::static_vector<non_trivial, 0_bi>>);
static_assert(!bounded::trivially_copy_constructible<containers::static_vector<non_trivial, 1_bi>>);
static_assert(bounded::copy_constructible<containers::static_vector<non_trivial, 1_bi>>);

static_assert(bounded::trivially_destructible<containers::static_vector<int, 0_bi>>);
static_assert(bounded::trivially_destructible<containers::static_vector<int, 1_bi>>);
static_assert(bounded::trivially_destructible<containers::static_vector<non_trivial, 0_bi>>);
static_assert(!bounded::trivially_destructible<containers::static_vector<non_trivial, 1_bi>>);
static_assert(std::destructible<containers::static_vector<non_trivial, 1_bi>>);

static_assert(bounded::trivially_copy_assignable<containers::static_vector<int, 0_bi>>);
static_assert(bounded::trivially_copy_assignable<containers::static_vector<int, 1_bi>>);
static_assert(bounded::trivially_copy_assignable<containers::static_vector<non_trivial, 0_bi>>);
static_assert(!bounded::trivially_copy_assignable<containers::static_vector<non_trivial, 1_bi>>);
static_assert(bounded::copy_assignable<containers::static_vector<non_trivial, 1_bi>>);

static_assert(bounded::trivially_move_assignable<containers::static_vector<int, 0_bi>>);
static_assert(bounded::trivially_move_assignable<containers::static_vector<int, 1_bi>>);
static_assert(bounded::trivially_move_assignable<containers::static_vector<non_trivial, 0_bi>>);
static_assert(!bounded::trivially_move_assignable<containers::static_vector<non_trivial, 1_bi>>);
static_assert(bounded::move_assignable<containers::static_vector<non_trivial, 1_bi>>);

template<typename T>
using test_static_vector = containers::static_vector<T, 40_bi>;

static_assert(test_static_vector<int>::capacity() == 40_bi);

static_assert(containers_test::test_sequence_container<test_static_vector<int>>());
static_assert(containers_test::test_set_size<test_static_vector<int>>());

using index_type = containers::index_type<containers::static_vector<int, 10_bi>>;
static_assert(numeric_traits::min_value<index_type> == 0_bi);
static_assert(numeric_traits::max_value<index_type> == 9_bi);
static_assert(!containers::iterator<containers::static_vector<int, 6_bi>>);

static_assert(homogeneous_equals(
	containers::make_static_vector(containers::array({5, 3})),
	containers::static_vector<int, 2_bi>({5, 3})
));

static_assert(!bounded::constructible_from<containers::static_vector<int, 1_bi>, containers::c_array<int, 2> &&>);

TEST_CASE("static_vector", "[static_vector]") {
	containers_test::test_sequence_container<test_static_vector<bounded_test::integer>>();
	containers_test::test_set_size<test_static_vector<bounded_test::integer>>();
}
