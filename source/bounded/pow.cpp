// Copyright David Stone 2020.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <bounded/pow.hpp>
#include <bounded/detail/comparison_mixed.hpp>
#include <bounded/detail/class.hpp>

namespace {

static_assert(bounded::pow(bounded::constant<0>, bounded::constant<1>) == bounded::constant<0>);
static_assert(bounded::pow(bounded::constant<0>, bounded::constant<2>) == bounded::constant<0>);
static_assert(bounded::pow(bounded::constant<0>, bounded::constant<3>) == bounded::constant<0>);
static_assert(bounded::pow(bounded::constant<0>, bounded::constant<4>) == bounded::constant<0>);
static_assert(bounded::pow(bounded::constant<1>, bounded::constant<1>) == bounded::constant<1>);
static_assert(bounded::pow(bounded::constant<123>, bounded::constant<1>) == bounded::constant<123>);
static_assert(bounded::pow(bounded::constant<9235234>, bounded::constant<0>) == bounded::constant<1>);
static_assert(bounded::pow(bounded::constant<9>, bounded::constant<2>) == bounded::constant<81>);
static_assert(bounded::pow(bounded::constant<2>, bounded::constant<10>) == bounded::constant<1024>);
static_assert(bounded::pow(bounded::constant<3>, bounded::constant<5>) == bounded::constant<243>);
static_assert(bounded::pow(bounded::constant<4>, bounded::constant<4>) == bounded::constant<256>);
static_assert(bounded::pow(bounded::constant<2>, bounded::constant<63>) == bounded::constant<1ULL << 63ULL>);

} // namespace
