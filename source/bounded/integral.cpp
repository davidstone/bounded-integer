// Copyright David Stone 2023.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

export module bounded.integral;

import bounded.bounded_integer;

namespace bounded {

export template<typename T>
concept integral =
	builtin_integer<T> or
	bounded_integer<T> or
	detail::is_integral_constant_of_integral<T>;

} // namespace bounded

namespace {

enum e {
};

struct s {
};

static_assert(bounded::integral<int>);
static_assert(!bounded::integral<bool>);
static_assert(!bounded::integral<e>);
static_assert(!bounded::integral<s>);

} // namespace