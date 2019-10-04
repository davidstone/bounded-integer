// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <bounded/detail/to_integer.hpp>
#include <bounded/detail/literal.hpp>

#include "../homogeneous_equals.hpp"

#include <cstdint>

namespace {
using namespace bounded::literal;

static_assert(bounded::to_integer<0, 0>("0") == 0_bi);
static_assert(bounded::to_integer<-100, 100>("0") == 0_bi);
static_assert(bounded::to_integer<-100, 100>("1") == 1_bi);
static_assert(bounded::to_integer<-100, 100>("-1") == -1_bi);
static_assert(bounded::to_integer<0, 100>("50") == 50_bi);
static_assert(bounded::to_integer<-100, -4>("-60") == -60_bi);
static_assert(bounded::to_integer<99, 100>("100") == 100_bi);
static_assert(
	bounded::to_integer<
		static_cast<std::int64_t>(bounded::min_value<std::int32_t>) - 1,
		static_cast<std::uint64_t>(bounded::max_value<std::uint32_t>) + 1
	>("4294967296") == 4294967296_bi
);
static_assert(
	bounded::to_integer<
		static_cast<std::int64_t>(bounded::min_value<std::int32_t>) - 1,
		static_cast<std::uint64_t>(bounded::max_value<std::uint32_t>) + 1
	>("-2147483648") == -2147483648_bi
);

static_assert(homogeneous_equals(
	bounded::to_integer<156, 325>("256"),
	bounded::to_integer<bounded::integer<156, 325>>("256")
));

} // namespace
