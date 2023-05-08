// Copyright David Stone 2023.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

export module containers.to_string;

import containers.string;
import containers.to_any_string;

import bounded;

using namespace bounded::literal;
namespace containers {

export constexpr auto to_string(bounded::bounded_integer auto const x) -> containers::string {
	return ::containers::to_any_string<containers::string>(x);
}

export constexpr auto to_string(bounded::builtin_integer auto const x) -> containers::string {
	return ::containers::to_any_string<containers::string>(x);
}

} // namespace containers

static_assert(::containers::to_string(0_bi) == "0");
static_assert(::containers::to_string(1_bi) == "1");
static_assert(::containers::to_string(15_bi) == "15");
static_assert(::containers::to_string(-1_bi) == "-1");
static_assert(::containers::to_string(bounded::integer<-100, 100>(7_bi)) == "7");