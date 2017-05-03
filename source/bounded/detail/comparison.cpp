// Copyright David Stone 2017.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <bounded/detail/comparison.hpp>
#include <bounded/detail/literal.hpp>
#include <bounded/detail/arithmetic/unary_minus.hpp>

namespace {

using namespace bounded::literal;

constexpr auto value = bounded::integer<0, 1>(0_bi);

static_assert(std::is_same<std::true_type, decltype(0_bi == 0_bi)>{});
static_assert(std::is_same<std::false_type, decltype(0_bi == 1_bi)>{});

static_assert(std::is_same<std::false_type, decltype(value == 2_bi)>{});
static_assert(std::is_same<bool, decltype(value == 1_bi)>{});
static_assert(std::is_same<bool, decltype(value == 0_bi)>{});
static_assert(std::is_same<std::false_type, decltype(value == -1_bi)>{});

static_assert(std::is_same<std::true_type, decltype(value != 2_bi)>{});
static_assert(std::is_same<bool, decltype(value != 1_bi)>{});
static_assert(std::is_same<bool, decltype(value != 0_bi)>{});
static_assert(std::is_same<std::true_type, decltype(value != -1_bi)>{});

static_assert(std::is_same<std::true_type, decltype(value < 2_bi)>{});
static_assert(std::is_same<bool, decltype(value < 1_bi)>{});
static_assert(std::is_same<std::false_type, decltype(value < 0_bi)>{});
static_assert(std::is_same<std::false_type, decltype(value < -1_bi)>{});

static_assert(std::is_same<std::true_type, decltype(value <= 2_bi)>{});
static_assert(std::is_same<std::true_type, decltype(value <= 1_bi)>{});
static_assert(std::is_same<bool, decltype(value <= 0_bi)>{});
static_assert(std::is_same<std::false_type, decltype(value <= -1_bi)>{});

static_assert(std::is_same<std::false_type, decltype(value > 2_bi)>{});
static_assert(std::is_same<std::false_type, decltype(value > 1_bi)>{});
static_assert(std::is_same<bool, decltype(value > 0_bi)>{});
static_assert(std::is_same<std::true_type, decltype(value > -1_bi)>{});

static_assert(std::is_same<std::false_type, decltype(value >= 2_bi)>{});
static_assert(std::is_same<bool, decltype(value >= 1_bi)>{});
static_assert(std::is_same<std::true_type, decltype(value >= 0_bi)>{});
static_assert(std::is_same<std::true_type, decltype(value >= -1_bi)>{});

}	// namespace
