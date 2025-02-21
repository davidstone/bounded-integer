// Copyright David Stone 2019.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

module;

#include <catch2/catch_test_macros.hpp>

#include <bounded/arithmetic/common_arithmetic.hpp>
#include <bounded/assert.hpp>

#include <operators/arrow.hpp>

export module containers.test.take;

import containers.algorithms.compare;

import containers.array;
import containers.begin_end;
import containers.common_iterator_functions;
import containers.iter_difference_t;
import containers.iterator_t;
import containers.take;

import bounded;
import std_module;

namespace {

using namespace bounded::literal;

using sized_range_t = containers::array<int, 8_bi>;
constexpr auto sized_range = sized_range_t{0, 1, 2, 3, 4, 5, 6, 7};

inline namespace adl {

BOUNDED_COMMON_ARITHMETIC()

struct unsized_range_t {
	struct const_iterator {
		using underlying_iterator = containers::iterator_t<sized_range_t const>;
		using difference_type = containers::iter_difference_t<underlying_iterator>;
		constexpr explicit const_iterator(underlying_iterator const it):
			m_it(it)
		{
		}
		constexpr auto & operator*() const {
			return *m_it;
		}
		OPERATORS_ARROW_DEFINITIONS

		friend constexpr auto operator+(const_iterator const lhs, bounded::constant_t<1> const rhs) {
			return const_iterator(lhs.m_it + rhs);
		}

		friend auto operator==(const_iterator, const_iterator) -> bool = default;

	private:
		underlying_iterator m_it;
	};
	constexpr auto begin() const {
		return const_iterator(containers::begin(sized_range));
	}
	constexpr auto end() const {
		return const_iterator(containers::end(sized_range));
	}
};
constexpr auto unsized_range = unsized_range_t();

struct input_range_t {
	struct iterator {
		using underlying_iterator = containers::iterator_t<sized_range_t const>;
		using difference_type = containers::iter_difference_t<underlying_iterator>;
		constexpr iterator(underlying_iterator const it, underlying_iterator const last):
			m_it(it),
			m_last(last)
		{
		}
		iterator(iterator &&) = default;
		iterator(iterator const &) = delete;
		auto operator=(iterator &&) -> iterator & = default;
		auto operator=(iterator const &) -> iterator & = delete;

		constexpr auto operator*() const -> auto & {
			return *m_it;
		}
		OPERATORS_ARROW_DEFINITIONS

		friend constexpr auto operator+(iterator lhs, bounded::constant_t<1>) {
			++lhs.m_it;
			return lhs;
		}

		friend constexpr auto operator==(iterator const & lhs, std::default_sentinel_t) -> bool {
			return lhs.m_it == lhs.m_last;
		}

	private:
		underlying_iterator m_it;
		underlying_iterator m_last;
	};

	input_range_t() = default;
	input_range_t(input_range_t &&) = default;
	input_range_t(input_range_t const &) = delete;
	auto operator=(input_range_t &&) -> input_range_t & = default;
	auto operator=(input_range_t const &) -> input_range_t & = delete;

	constexpr auto begin() && {
		return iterator(containers::begin(sized_range), containers::end(sized_range));
	}
	constexpr auto end() const {
		return std::default_sentinel;
	}
};

} // inline namespace adl

constexpr auto test_take(auto get_source) {
	BOUNDED_ASSERT(containers::equal(containers::take(get_source(), 8_bi), sized_range));
	BOUNDED_ASSERT(containers::equal(containers::take(get_source(), 4_bi), containers::array{0, 1, 2, 3}));
	BOUNDED_ASSERT(containers::equal(containers::take(get_source(), 1_bi), containers::array{0}));
	BOUNDED_ASSERT(containers::equal(containers::take(get_source(), 0_bi), containers::array<int, 0_bi>()));
	BOUNDED_ASSERT(containers::equal(containers::take(get_source(), 12_bi), sized_range));
	return true;
}
static_assert(test_take([] -> auto const & { return sized_range; }));
static_assert(containers::take(sized_range, 4_bi)[3_bi] == 3);
static_assert(test_take([] { return unsized_range; }));
static_assert(test_take(bounded::construct<input_range_t>));

template<typename Range, auto index>
concept takeable = requires(Range range) { containers::take(range, index); };

static_assert(!takeable<sized_range_t, -1_bi>);
static_assert(!takeable<unsized_range_t, -1_bi>);
static_assert(!takeable<input_range_t, -1_bi>);


constexpr auto test_check_size_not_greater_than(auto get_source) {
	BOUNDED_ASSERT(containers::equal(
		containers::check_size_not_greater_than(get_source(), 8_bi),
		sized_range
	));
	BOUNDED_ASSERT(containers::equal(
		containers::check_size_not_greater_than(get_source(), 12_bi),
		sized_range
	));
	return true;
}
static_assert(test_check_size_not_greater_than([] -> auto const & { return sized_range; }));
static_assert(containers::check_size_not_greater_than(sized_range, 8_bi)[3_bi] == 3);
static_assert(test_check_size_not_greater_than([] { return unsized_range; }));
static_assert(test_check_size_not_greater_than(bounded::construct<input_range_t>));

template<typename Range, auto index>
concept check_sizable = requires(Range range) { containers::check_size_not_greater_than(std::move(range), index); };

static_assert(!check_sizable<sized_range_t, -1_bi>);
static_assert(!check_sizable<unsized_range_t, -1_bi>);
static_assert(!check_sizable<input_range_t, -1_bi>);
static_assert(!check_sizable<sized_range_t, 1_bi>);
static_assert(check_sizable<unsized_range_t, 1_bi>);
static_assert(check_sizable<input_range_t, 1_bi>);

TEST_CASE("check_size_not_greater_than fails with count of 0 for sized range", "[check_size_not_greater_than]") {
	constexpr auto count = bounded::integer<0, 8>(0_bi);
	CHECK_THROWS(containers::check_size_not_greater_than(sized_range, count));
}
TEST_CASE("check_size_not_greater_than fails with count of 1 for sized range", "[check_size_not_greater_than]") {
	constexpr auto count = bounded::integer<0, 8>(1_bi);
	CHECK_THROWS(containers::check_size_not_greater_than(sized_range, count));
}

TEST_CASE("check_size_not_greater_than fails with count of 0 for unsized range", "[check_size_not_greater_than]") {
	CHECK_THROWS([] {
		for (auto const & _ : containers::check_size_not_greater_than(unsized_range, 0_bi)) {
		}
	}());
}
TEST_CASE("check_size_not_greater_than fails with count of 1 for unsized range", "[check_size_not_greater_than]") {
	CHECK_THROWS([] {
		for (auto const & _ : containers::check_size_not_greater_than(unsized_range, 1_bi)) {
		}
	}());
}

TEST_CASE("check_size_not_greater_than fails with count of 0 for input range", "[check_size_not_greater_than]") {
	CHECK_THROWS([] {
		auto range = containers::check_size_not_greater_than(input_range_t(), 0_bi);
		auto const last = containers::end(range);
		for (auto it = containers::begin(std::move(range)); it != last; ++it) {
		}
	}());
}
TEST_CASE("check_size_not_greater_than fails with count of 1 for input range", "[check_size_not_greater_than]") {
	CHECK_THROWS([] {
		auto range = containers::check_size_not_greater_than(input_range_t(), 1_bi);
		auto const last = containers::end(range);
		for (auto it = containers::begin(std::move(range)); it != last; ++it) {
		}
	}());
}

} // namespace