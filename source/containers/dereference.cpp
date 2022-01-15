// Copyright David Stone 2021.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <containers/dereference.hpp>

#include <containers/array.hpp>

namespace {

static_assert(containers::detail::should_replace_copy_with_move<containers::array<int, 0>>);
static_assert(containers::detail::should_replace_copy_with_move<containers::array<int, 0> &&>);
static_assert(!containers::detail::should_replace_copy_with_move<containers::array<int, 0> const &>);
static_assert(!containers::detail::should_replace_copy_with_move<containers::array<int, 0> &>);

} // namespace