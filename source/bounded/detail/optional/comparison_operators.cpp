// Copyright David Stone 2015.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <bounded/detail/optional/comparison_operators.hpp>

#include <bounded/detail/literal.hpp>

#include <bounded/detail/arithmetic/unary_minus.hpp>

namespace {
using namespace bounded::literal;

using integer = bounded::integer<0, 10>;

constexpr bounded::optional<integer> uninitialized;

static_assert(uninitialized == bounded::none, "Fail to compare uninitialized optional to none_t ==.");
static_assert(uninitialized != 0_bi, "Fail to compare uninitialized optional to a value with !=.");
static_assert(uninitialized < -1_bi, "Fail to compare uninitialized optional to a value with <.");


constexpr bounded::optional<integer> initialized(5_bi);

static_assert(initialized != bounded::none, "Fail to compare initialized optional to none_t !=.");
static_assert(initialized == 5_bi, "Fail to compare initialized optional to a value with ==.");
static_assert(initialized != 4_bi, "Fail to compare initialized optional to a value with !=.");
static_assert(initialized < 100_bi, "Fail to compare initialized optional to a value with <.");
static_assert(initialized > -5_bi, "Fail to compare initialized optional to a value with <.");
static_assert(initialized <= 5_bi, "Fail to compare initialized optional to a value with <=.");
static_assert(initialized <= 6_bi, "Fail to compare initialized optional to a value with <=.");
static_assert(initialized >= 5_bi, "Fail to compare initialized optional to a value with >=.");
static_assert(initialized >= 0_bi, "Fail to compare initialized optional to a value with >=.");

static_assert(bounded::none != initialized, "Fail to compare initialized optional to none_t !=.");
static_assert(5_bi == initialized, "Fail to compare initialized optional to a value with ==.");
static_assert(17_bi != initialized, "Fail to compare initialized optional to a value with !=.");
static_assert(2_bi < initialized, "Fail to compare initialized optional to a value with <.");
static_assert(123456 > initialized, "Fail to compare initialized optional to a value with >.");
static_assert(5_bi <= initialized, "Fail to compare initialized optional to a value with <=.");
static_assert(-25 <= initialized, "Fail to compare initialized optional to a value with <=.");
static_assert(5_bi >= initialized, "Fail to compare initialized optional to a value with >=.");
static_assert(300U >= initialized, "Fail to compare initialized optional to a value with >=.");

}	// namespace
