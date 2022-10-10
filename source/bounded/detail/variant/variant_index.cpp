// Copyright David Stone 2022.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <bounded/detail/variant/variant_index.hpp>

#include <bounded/detail/literal.hpp>

#include <bounded/concepts.hpp>

namespace {

using namespace bounded::literal;

static_assert(!bounded::constructible_from<bounded::variant_index<>, bounded::constant_t<0>>);

static_assert(bounded::constructible_from<bounded::variant_index<int>, bounded::constant_t<0>>);
static_assert(!bounded::constructible_from<bounded::variant_index<int>, bounded::constant_t<1>>);

static_assert(bounded::constructible_from<bounded::variant_index<int, long>, bounded::constant_t<0>>);
static_assert(bounded::constructible_from<bounded::variant_index<int, long>, bounded::constant_t<1>>);
static_assert(!bounded::constructible_from<bounded::variant_index<int, long>, bounded::constant_t<2>>);

static_assert(bounded::constructible_from<bounded::variant_index<int, int>, bounded::constant_t<0>>);
static_assert(bounded::constructible_from<bounded::variant_index<int, int>, bounded::constant_t<1>>);
static_assert(!bounded::constructible_from<bounded::variant_index<int, int>, bounded::constant_t<2>>);

static_assert(bounded::variant_index<int>(0_bi).integer() == 0_bi);

static_assert(bounded::variant_index<int, long>(0_bi).integer() == 0_bi);
static_assert(bounded::variant_index<int, long>(1_bi).integer() == 1_bi);
static_assert(bounded::variant_index<int, int>(0_bi).integer() == 0_bi);
static_assert(bounded::variant_index<int, int>(1_bi).integer() == 1_bi);

static_assert(bounded::variant_index<int>(0_bi) == bounded::variant_index<int>(0_bi));
static_assert(bounded::variant_index<int>(0_bi) == 0_bi);
static_assert(bounded::variant_index<int>(0_bi) == bounded::type<int>);

static_assert(bounded::variant_index<int, long>(0_bi) == bounded::variant_index<int, long>(0_bi));
static_assert(bounded::variant_index<int, long>(0_bi) != bounded::variant_index<int, long>(1_bi));
static_assert(bounded::variant_index<int, long>(1_bi) == bounded::variant_index<int, long>(1_bi));
static_assert(bounded::variant_index<int, long>(0_bi) == 0_bi);
static_assert(bounded::variant_index<int, long>(1_bi) == 1_bi);
static_assert(bounded::variant_index<int, long>(0_bi) == bounded::type<int>);
static_assert(bounded::variant_index<int, long>(0_bi) != bounded::type<long>);
static_assert(bounded::variant_index<int, long>(1_bi) == bounded::type<long>);
static_assert(bounded::variant_index<int, long>(1_bi) != bounded::type<int>);

static_assert(bounded::variant_index<int, int>(0_bi) == bounded::variant_index<int, int>(0_bi));
static_assert(bounded::variant_index<int, int>(0_bi) != bounded::variant_index<int, int>(1_bi));
static_assert(bounded::variant_index<int, int>(1_bi) == bounded::variant_index<int, int>(1_bi));
static_assert(bounded::variant_index<int, int>(0_bi) == 0_bi);
static_assert(bounded::variant_index<int, int>(1_bi) == 1_bi);
static_assert(bounded::variant_index<int, int>(0_bi) == bounded::type<int>);
static_assert(bounded::variant_index<int, int>(1_bi) == bounded::type<int>);

} // namespace
