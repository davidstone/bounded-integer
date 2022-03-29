// Copyright David Stone 2015.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <bounded/detail/is_bounded_integer.hpp>

#include <bounded/detail/class.hpp>

#include <cstddef>

namespace {

struct s {
};

static_assert(bounded::bounded_integer<bounded::integer<0, 0>>);
static_assert(bounded::bounded_integer<bounded::integer<0, 1>>);
static_assert(!bounded::bounded_integer<int>);
static_assert(!bounded::bounded_integer<bool>);
static_assert(!bounded::bounded_integer<std::byte>);
static_assert(!bounded::bounded_integer<s>);

static_assert(bounded::integral<bounded::integer<0, 0>>);
static_assert(bounded::integral<bounded::integer<0, 1>>);
static_assert(bounded::integral<int>);
static_assert(!bounded::integral<bool>);
static_assert(!bounded::integral<std::byte>);
static_assert(!bounded::integral<s>);

static_assert(bounded::isomorphic_to_integral<bounded::integer<0, 0>>);
static_assert(bounded::isomorphic_to_integral<bounded::integer<0, 1>>);
static_assert(bounded::isomorphic_to_integral<int>);
static_assert(bounded::isomorphic_to_integral<bool>);
static_assert(bounded::isomorphic_to_integral<std::byte>);
static_assert(!bounded::isomorphic_to_integral<s>);

} // namespace