// Copyright David Stone 2019.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <containers/take.hpp>

#include <containers/algorithms/compare.hpp>

#include <containers/array.hpp>

#include <operators/operators.hpp>

#include <catch2/catch_test_macros.hpp>

namespace {

using namespace bounded::literal;

using sized_range_t = containers::array<int, 8>;
constexpr auto sized_range = sized_range_t{0, 1, 2, 3, 4, 5, 6, 7};

inline namespace adl {

BOUNDED_COMMON_ARITHMETIC

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

} // inline namespace adl

constexpr auto test_take(auto const & source) {
	BOUNDED_ASSERT(containers::equal(containers::take(source, 8_bi), sized_range));
	BOUNDED_ASSERT(containers::equal(containers::take(source, 4_bi), containers::array{0, 1, 2, 3}));
	BOUNDED_ASSERT(containers::equal(containers::take(source, 1_bi), containers::array{0}));
	BOUNDED_ASSERT(containers::equal(containers::take(source, 0_bi), containers::array<int, 0>()));
	BOUNDED_ASSERT(containers::equal(containers::take(source, 12_bi), sized_range));
	return true;
}
static_assert(test_take(sized_range));
static_assert(containers::take(sized_range, 4_bi)[3_bi] == 3);
static_assert(test_take(unsized_range));

template<typename Range, auto index>
concept takeable = requires(Range range) { containers::take(range, index); };

static_assert(!takeable<sized_range_t, -1_bi>);
static_assert(!takeable<unsized_range_t, -1_bi>);


constexpr auto test_check_size_not_greater_than(auto const & source) {
	BOUNDED_ASSERT(containers::equal(containers::check_size_not_greater_than(source, 8_bi), sized_range));
	BOUNDED_ASSERT(containers::equal(containers::check_size_not_greater_than(source, 12_bi), sized_range));
	return true;
}
static_assert(test_check_size_not_greater_than(sized_range));
static_assert(containers::check_size_not_greater_than(sized_range, 8_bi)[3_bi] == 3);
static_assert(test_check_size_not_greater_than(unsized_range));

template<typename Range, auto index>
concept check_sizable = requires(Range range) { containers::check_size_not_greater_than(range, index); };

static_assert(!check_sizable<sized_range_t, -1_bi>);
static_assert(!check_sizable<unsized_range_t, -1_bi>);
static_assert(!check_sizable<sized_range_t, 1_bi>);
static_assert(check_sizable<unsized_range_t, 1_bi>);

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
		for ([[maybe_unused]] auto const & _ : containers::check_size_not_greater_than(unsized_range, 0_bi)) {
		}
	}());
}
TEST_CASE("check_size_not_greater_than fails with count of 1 for unsized range", "[check_size_not_greater_than]") {
	CHECK_THROWS([] {
		for ([[maybe_unused]] auto const & _ : containers::check_size_not_greater_than(unsized_range, 1_bi)) {
		}
	}());
}

} // namespace