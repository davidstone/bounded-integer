// Copyright David Stone 2026.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

export module bounded.bounded_by_range;

import bounded.builtin_min_max_value;
import bounded.isomorphic_to_integral;
import bounded.safe_compare;

import numeric_traits;
import std_module;

namespace bounded {

// At least one value of `T` is in the range `[minimum, maximum]`
export template<typename T, auto minimum, auto maximum>
concept overlapping_integer =
	isomorphic_to_integral<T> and
	safe_compare(builtin_min_value<T>, maximum) <= 0 and safe_compare(minimum, builtin_max_value<T>) <= 0;

// All values of `T` are in the range `[minimum, maximum]`
export template<typename T, auto minimum, auto maximum>
concept bounded_by_range =
	overlapping_integer<T, minimum, maximum> and
	safe_compare(minimum, builtin_min_value<T>) <= 0 and
	safe_compare(builtin_max_value<T>, maximum) <= 0;

} // namespace bounded

static_assert(bounded::overlapping_integer<std::int8_t, 0, 0>);
static_assert(bounded::overlapping_integer<std::int8_t, numeric_traits::min_value<std::int8_t>, numeric_traits::max_value<std::int8_t>>);
static_assert(bounded::overlapping_integer<std::int8_t, numeric_traits::min_value<std::int8_t> - 1, 0>);
static_assert(bounded::overlapping_integer<std::int8_t, 0, numeric_traits::max_value<std::int8_t> + 1>);
static_assert(bounded::overlapping_integer<std::int8_t, numeric_traits::min_value<std::int8_t> - 1, numeric_traits::max_value<std::int8_t> + 1>);
static_assert(!bounded::overlapping_integer<std::int8_t, numeric_traits::min_value<std::int8_t> - 1, numeric_traits::min_value<std::int8_t> - 1>);
static_assert(!bounded::overlapping_integer<std::int8_t, numeric_traits::max_value<std::int8_t> + 1, numeric_traits::max_value<std::int8_t> + 1>);

static_assert(!bounded::bounded_by_range<std::int8_t, 0, 0>);
static_assert(bounded::bounded_by_range<std::int8_t, numeric_traits::min_value<std::int8_t>, numeric_traits::max_value<std::int8_t>>);
static_assert(!bounded::bounded_by_range<std::int8_t, numeric_traits::min_value<std::int8_t> - 1, 0>);
static_assert(!bounded::bounded_by_range<std::int8_t, 0, numeric_traits::max_value<std::int8_t> + 1>);
static_assert(bounded::bounded_by_range<std::int8_t, numeric_traits::min_value<std::int8_t> - 1, numeric_traits::max_value<std::int8_t> + 1>);
static_assert(!bounded::bounded_by_range<std::int8_t, numeric_traits::min_value<std::int8_t> - 1, numeric_traits::min_value<std::int8_t> - 1>);
static_assert(!bounded::bounded_by_range<std::int8_t, numeric_traits::max_value<std::int8_t> + 1, numeric_traits::max_value<std::int8_t> + 1>);
