// Copyright David Stone 2017.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <bounded/detail/class.hpp>
#include <bounded/detail/comparison.hpp>

namespace {

// Intentionally not constexpr
auto const zero = bounded::constant<0>;
auto const one = bounded::constant<1>;

static_assert(zero == zero);
static_assert(zero != one);
static_assert(zero < one);
static_assert(one > zero);
static_assert(zero <= one);
static_assert(one >= zero);

}	// namespace
