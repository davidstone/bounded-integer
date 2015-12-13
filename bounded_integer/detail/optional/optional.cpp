// Copyright David Stone 2015.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include "optional.hpp"

#include <type_traits>

namespace {

using type = int;

static_assert(std::is_convertible<type, bounded::optional<type>>::value, "Cannot construct optional of identical type.");

}	// namespace
