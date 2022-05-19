// Copyright David Stone 2021.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <containers/can_set_size.hpp>

#include <containers/array.hpp>
#include <containers/vector.hpp>

namespace {

static_assert(!containers::detail::can_set_size<int>);
static_assert(!containers::detail::can_set_size<containers::array<int, bounded::constant<1>> &>);
static_assert(containers::detail::can_set_size<containers::vector<int> &>);

} // namespace