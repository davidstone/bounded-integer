// Copyright David Stone 2015.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <bounded/hash.hpp>

namespace {

using integer = bounded::integer<0, 0>;

static_assert(std::same_as<
	decltype(std::hash<integer>()(integer())),
	decltype(std::hash<integer::underlying_type>()(integer::underlying_type()))
>);

} // namespace