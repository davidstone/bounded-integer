// Copyright David Stone 2023.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

export module bounded.bounded_integer;

import bounded.is_bounded_integer;

namespace bounded {

export template<typename T>
concept bounded_integer = is_bounded_integer<T>;

} // namespace bounded

namespace {

enum e {
};

struct s {
};

static_assert(!bounded::bounded_integer<int>);
static_assert(!bounded::bounded_integer<bool>);
static_assert(!bounded::bounded_integer<e>);
static_assert(!bounded::bounded_integer<s>);

} // namespace