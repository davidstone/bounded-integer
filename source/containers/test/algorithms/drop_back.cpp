// Copyright David Stone 2026.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

module;

#include <doctest/doctest.h>

#include <bounded/arithmetic/common_arithmetic.hpp>
#include <bounded/assert.hpp>

#include <operators/arrow.hpp>

export module containers.test.algorithms.drop_back;

import containers.algorithms.compare;
import containers.algorithms.drop_back;

import containers.array;
import containers.begin_end;
import containers.common_iterator_functions;
import containers.contiguous_range;
import containers.iter_difference_t;
import containers.iterator_t;
import containers.size;
import containers.sized_range;
import containers.span;
import containers.static_vector;

import bounded;
import std_module;

namespace {

using namespace bounded::literal;

using sized_range_t = containers::static_vector<int, 4_bi>;
constexpr auto sized_range = sized_range_t({0, 1, 2});

static_assert(containers::contiguous_range<decltype(containers::drop_back_exactly(sized_range, 0_bi))>);
static_assert(containers::contiguous_range<decltype(containers::drop_back_exactly(sized_range, 1_bi))>);
static_assert(containers::contiguous_range<decltype(containers::drop_back_exactly(sized_range, 3_bi))>);

inline namespace adl {

BOUNDED_COMMON_ARITHMETIC()

struct input_iterator {
	using underlying_iterator = containers::iterator_t<sized_range_t const>;
	using difference_type = containers::iter_difference_t<underlying_iterator>;
	constexpr input_iterator(underlying_iterator const it, underlying_iterator const last):
		m_it(it),
		m_last(last)
	{
	}
	input_iterator(input_iterator &&) = default;
	input_iterator(input_iterator const &) = delete;
	auto operator=(input_iterator &&) -> input_iterator & = default;
	auto operator=(input_iterator const &) -> input_iterator & = delete;

	constexpr auto operator*() const -> auto & {
		return *m_it;
	}
	OPERATORS_ARROW_DEFINITIONS

	friend constexpr auto operator+(input_iterator lhs, bounded::constant_t<1>) {
		++lhs.m_it;
		return lhs;
	}

	friend constexpr auto operator==(input_iterator const & lhs, std::default_sentinel_t) -> bool {
		return lhs.m_it == lhs.m_last;
	}

private:
	underlying_iterator m_it;
	underlying_iterator m_last;
};

struct sized_input_range_t {
	sized_input_range_t() = default;
	sized_input_range_t(sized_input_range_t &&) = default;
	sized_input_range_t(sized_input_range_t const &) = delete;
	auto operator=(sized_input_range_t &&) -> sized_input_range_t & = default;
	auto operator=(sized_input_range_t const &) -> sized_input_range_t & = delete;

	constexpr auto begin() && {
		return input_iterator(containers::begin(sized_range), containers::end(sized_range));
	}
	constexpr auto end() const {
		return std::default_sentinel;
	}
	constexpr auto size() const {
		return containers::size(sized_range);
	}
};
static_assert(containers::sized_range<sized_input_range_t>);

} // inline namespace adl

constexpr auto test_drop_back_exactly(auto get_source) {
	BOUNDED_ASSERT(containers::equal(
		containers::drop_back_exactly(get_source(), 0_bi),
		containers::array{0, 1, 2}
	));
	BOUNDED_ASSERT(containers::equal(
		containers::drop_back_exactly(get_source(), 1_bi),
		containers::array{0, 1}
	));
	BOUNDED_ASSERT(containers::equal(
		containers::drop_back_exactly(get_source(), 2_bi),
		containers::array{0}
	));
	BOUNDED_ASSERT(containers::equal(
		containers::drop_back_exactly(get_source(), 3_bi),
		containers::array<int, 0_bi>()
	));
	static_assert(!requires { containers::drop_back_exactly(get_source(), -1_bi); });
	static_assert(requires { containers::drop_back_exactly(get_source(), 4_bi); });
	static_assert(
		std::same_as<decltype(get_source()), containers::span<int const>> or
		!requires { containers::drop_back_exactly(get_source(), 5_bi); }
	);
	return true;
}

static_assert(test_drop_back_exactly([] -> auto const & { return sized_range; }));
static_assert(containers::drop_back_exactly(sized_range, 1_bi)[0_bi] == 0);
static_assert(test_drop_back_exactly([] { return containers::span<int const>(sized_range); }));
static_assert(containers::drop_back_exactly(containers::span<int const>(sized_range), 1_bi)[0] == 0);
static_assert(test_drop_back_exactly(bounded::construct<sized_input_range_t>));
// TODO
// static_assert(containers::size(containers::drop_back_exactly(sized_input_range_t(), 2_bi)) == 1_bi);

constexpr auto too_large_count = bounded::integer<0, 4>(4_bi);
TEST_CASE("drop_back_exactly: drop_back_exactly fails with too-large count for sized range") {
	CHECK_THROWS(containers::drop_back_exactly(sized_range, too_large_count));
}
TEST_CASE("drop_back_exactly: drop_back_exactly fails with too-large count for sized input range") {
	CHECK_THROWS(containers::drop_back_exactly(sized_input_range_t(), too_large_count));
}

} // namespace