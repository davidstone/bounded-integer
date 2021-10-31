// Copyright David Stone 2021.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <containers/appendable_from_capacity.hpp>

#include <containers/array.hpp>
#include <containers/vector.hpp>

namespace {

static_assert(!containers::detail::appendable_from_capacity<int>);
static_assert(!containers::detail::appendable_from_capacity<containers::array<int, 1> &>);
static_assert(containers::detail::appendable_from_capacity<containers::vector<int> &>);

} // namespace