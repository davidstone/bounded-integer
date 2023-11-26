// Copyright David Stone 2023.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

export module bounded.isomorphic_to_integral;

import bounded.bounded_integer;

import std_module;

namespace bounded {

export template<typename T>
concept isomorphic_to_integral =
	integral<T> or
	std::is_enum_v<T> or
	std::same_as<T, bool>;

} // namespace bounded

namespace {

struct s {
};

static_assert(bounded::isomorphic_to_integral<int>);
static_assert(bounded::isomorphic_to_integral<bool>);
static_assert(bounded::isomorphic_to_integral<std::byte>);
static_assert(!bounded::isomorphic_to_integral<s>);

} // namespace