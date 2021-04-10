// Copyright David Stone 2017.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/common_functions.hpp>
#include <containers/is_iterator.hpp>

#include <bounded/integer.hpp>

// The strange namespacing and using declarations here are to ensure these
// functions are picked up by ADL for types defined in namespaces ::container or
// ::container::detail

namespace containers {
namespace detail {
namespace common {

constexpr auto operator+(bounded::integral auto const offset, iterator auto const it) {
	return it + offset;
}

}	// namespace common

using containers::detail::common::operator+;
using bounded::operator+=;
using bounded::operator-=;
using bounded::operator++;
using bounded::operator--;

}	// namespace detail

using containers::detail::common::operator+;
using bounded::operator+=;
using bounded::operator-=;
using bounded::operator++;
using bounded::operator--;

}	// namespace containers
