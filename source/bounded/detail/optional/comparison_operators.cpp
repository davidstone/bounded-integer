// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <bounded/detail/optional/comparison_operators.hpp>

#include <bounded/detail/literal.hpp>

#include <bounded/detail/arithmetic/unary_minus.hpp>

namespace {
using namespace bounded::literal;

using integer = bounded::integer<0, 10>;
using optional = bounded::optional<integer>;

constexpr optional uninitialized;

static_assert(uninitialized == bounded::none, "Fail to compare uninitialized optional to none_t.");
static_assert(uninitialized == optional(bounded::none), "Fail to compare uninitialized optional to uninitialized optional.");
static_assert(uninitialized != optional(0_bi), "Fail to compare uninitialized optional to a value.");
static_assert(bounded::none == uninitialized, "Fail to compare uninitialized optional to none_t.");
static_assert(optional(bounded::none) == uninitialized, "Fail to compare uninitialized optional to uninitialized optional.");
static_assert(optional(0_bi) != uninitialized, "Fail to compare uninitialized optional to a value.");

constexpr optional initialized(5_bi);

static_assert(initialized != bounded::none, "Fail to compare initialized optional to none_t.");
static_assert(initialized != optional(bounded::none), "Fail to compare initialized optional to uninitialized optional.");
static_assert(initialized == optional(5_bi), "Fail to compare initialized optional to a value.");
static_assert(initialized != optional(4_bi), "Fail to compare initialized optional to a value.");

static_assert(bounded::none != initialized, "Fail to compare initialized optional to none_t.");
static_assert(optional(5_bi) == initialized, "Fail to compare initialized optional to a value.");
static_assert(optional(1_bi) != initialized, "Fail to compare initialized optional to a value.");

}	// namespace
