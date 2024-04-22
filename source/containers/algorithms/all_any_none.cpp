// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

module;

#include <operators/forward.hpp>

export module containers.algorithms.all_any_none;

import containers.algorithms.find;
import containers.array;
import containers.begin_end;
import containers.c_array;
import containers.range;
import containers.range_reference_t;

import bounded;
import std_module;

using namespace bounded::literal;

namespace containers {

export constexpr auto all(range auto && range, auto predicate) -> bool {
	return ::containers::find_if_not(OPERATORS_FORWARD(range), predicate) == containers::end(OPERATORS_FORWARD(range));
}
export template<typename T, std::size_t size>
constexpr auto all(c_array<T, size> && range, auto predicate) -> bool {
	return ::containers::all(range, predicate);
}
export constexpr auto all(empty_c_array_parameter, auto) -> bool {
	return true;
}

export template<range Range>
constexpr auto all_equal(
	Range && range,
	bounded::equality_comparable<range_reference_t<Range>> auto && value
) -> bool {
	return ::containers::all(OPERATORS_FORWARD(range), bounded::equal_to(OPERATORS_FORWARD(value)));
}
export template<typename T, std::size_t size>
constexpr auto all_equal(
	c_array<T, size> && range,
	bounded::equality_comparable<T &> auto && value
) -> bool {
	return ::containers::all_equal(range, OPERATORS_FORWARD(value));
}
export constexpr auto all_equal(empty_c_array_parameter, auto &&) -> bool {
	return true;
}

export constexpr auto any(range auto && range, auto predicate) -> bool {
	return ::containers::find_if(OPERATORS_FORWARD(range), predicate) != containers::end(OPERATORS_FORWARD(range));
}
export template<typename T, std::size_t size>
constexpr auto any(c_array<T, size> && range, auto predicate) -> bool {
	return ::containers::any(range, predicate);
}
export constexpr auto any(empty_c_array_parameter, auto) -> bool {
	return false;
}

export template<range Range>
constexpr auto any_equal(
	Range && range,
	bounded::equality_comparable<range_reference_t<Range>> auto && value
) -> bool {
	return ::containers::any(OPERATORS_FORWARD(range), bounded::equal_to(OPERATORS_FORWARD(value)));
}
export template<typename T, std::size_t size>
constexpr auto any_equal(
	c_array<T, size> && range,
	bounded::equality_comparable<T &> auto && value
) -> bool {
	return ::containers::any_equal(range, OPERATORS_FORWARD(value));
}
export constexpr auto any_equal(empty_c_array_parameter, auto &&) -> bool {
	return false;
}

export constexpr auto none(range auto && range, auto predicate) -> bool {
	return ::containers::find_if(OPERATORS_FORWARD(range), predicate) == containers::end(OPERATORS_FORWARD(range));
}
export template<typename T, std::size_t size>
constexpr auto none(c_array<T, size> && range, auto predicate) -> bool {
	return ::containers::none(range, predicate);
}
export constexpr auto none(empty_c_array_parameter, auto) -> bool {
	return true;
}

export template<range Range>
constexpr auto none_equal(
	Range && range,
	bounded::equality_comparable<range_reference_t<Range>> auto && value
) -> bool {
	return ::containers::none(OPERATORS_FORWARD(range), bounded::equal_to(OPERATORS_FORWARD(value)));
}
export template<typename T, std::size_t size>
constexpr auto none_equal(
	c_array<T, size> && range,
	bounded::equality_comparable<T &> auto && value
) -> bool {
	return ::containers::none_equal(range, OPERATORS_FORWARD(value));
}
export constexpr auto none_equal(empty_c_array_parameter, auto &&) -> bool {
	return true;
}

} // namespace containers

static_assert(containers::all_equal(containers::array<int, 0_bi>{}, 2));
static_assert(containers::all_equal(containers::array{2, 2, 2}, 2));
static_assert(!containers::all_equal(containers::array{2, 1, 2}, 2));
static_assert(containers::all_equal({}, 2));
static_assert(containers::all_equal({2, 2, 2}, 2));
static_assert(!containers::all_equal({2, 1, 2}, 2));

static_assert(!containers::any_equal(containers::array<int, 0_bi>{}, 2));
static_assert(containers::any_equal(containers::array{2, 2, 2}, 2));
static_assert(containers::any_equal(containers::array{1, 2, 3}, 2));
static_assert(containers::any_equal(containers::array{1, 1, 2}, 2));
static_assert(!containers::any_equal(containers::array{1, 1, 3}, 2));
static_assert(!containers::any_equal({}, 2));
static_assert(containers::any_equal({1, 1, 2}, 2));
static_assert(!containers::any_equal({1, 1, 3}, 2));

static_assert(containers::none_equal(containers::array<int, 0_bi>{}, 4));
static_assert(containers::none_equal(containers::array{1, 2, 3}, 4));
static_assert(containers::none_equal(containers::array{1, 1, 1}, 4));
static_assert(!containers::none_equal(containers::array{1, 1, 1, 4}, 4));
static_assert(containers::none_equal({}, 4));
static_assert(containers::none_equal({1}, 4));
static_assert(!containers::none_equal({1, 4}, 4));
