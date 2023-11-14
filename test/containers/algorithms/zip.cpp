// Copyright David Stone 2023.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

module;

#include <std_module/prelude.hpp>
#include <catch2/catch_test_macros.hpp>

#include <bounded/assert.hpp>

export module containers.test.algorithms.zip;

import containers.algorithms.compare;
import containers.algorithms.zip;

import containers.array;
import containers.common_iterator_functions;
import containers.is_empty;
import containers.is_iterator;
import containers.iterator_t;
import containers.range_view;

import bounded;
import tv;

using containers::operator+;
using containers::operator+=;
using containers::operator-=;
using containers::operator++;
using containers::operator--;

using namespace bounded::literal;

static_assert(containers::iterator<containers::iterator_t<decltype(containers::zip())>>);

static_assert(containers::is_empty(containers::zip()));

constexpr auto a1 = containers::array({1, 2});
constexpr auto a2 = containers::array({3, 4});
constexpr auto expected = containers::array({tv::tuple(1, 3), tv::tuple(2, 4)});
static_assert(containers::equal(containers::zip(a1), a1));
static_assert(containers::equal(containers::zip(a1, a2), expected));


struct input_sentinel {
};
struct input_iterator {
	using difference_type = bounded::integer<-1000, 1000>;
	constexpr explicit input_iterator(int value):
		m_value(value)
	{
	}
	input_iterator(input_iterator &&) = default;
	input_iterator(input_iterator const &) = delete;
	auto operator=(input_iterator &&) & -> input_iterator & = default;
	auto operator=(input_iterator const &) & -> input_iterator & = delete;

	friend constexpr auto operator*(input_iterator const & it) -> int {
		return it.m_value;
	}
	friend constexpr auto operator+(input_iterator lhs, bounded::constant_t<1>) -> input_iterator {
		--lhs.m_value;
		return lhs;
	}
	friend constexpr auto operator==(input_iterator const & lhs, input_sentinel) {
		return lhs.m_value == 0;
	}
private:
	int m_value;
};

constexpr auto make(int value) {
	return containers::range_view(input_iterator(value), input_sentinel());
}

static_assert(containers::is_empty(containers::zip(make(0))));
static_assert(containers::equal(
	containers::zip(make(1)),
	containers::array({tv::tuple(1)})
));
static_assert(containers::equal(
	containers::zip(make(1), make(1)),
	containers::array({tv::tuple(1, 1)})
));
static_assert(containers::equal(
	containers::zip(make(2), a2),
	containers::array({tv::tuple(2, 3), tv::tuple(1, 4)})
));

TEST_CASE("unequal-sized zips throws with sized inputs", "[zip]") {
	CHECK_THROWS(containers::zip(
		containers::array<int, 0_bi>(),
		containers::array<int, 1_bi>()
	));
}
TEST_CASE("unequal-sized zips throws with unsized inputs", "[zip]") {
	auto zipped = containers::zip(
		make(0),
		make(1)
	);
	CHECK_THROWS(std::move(zipped).begin() != std::move(zipped).end());
}
TEST_CASE("unequal-sized zips throws with some sized some unsized inputs", "[zip]") {
	auto zipped = containers::zip(
		containers::array<int, 0_bi>(),
		make(1)
	);
	CHECK_THROWS(std::move(zipped).begin() != std::move(zipped).end());
}