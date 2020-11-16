// Copyright David Stone 2020.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <containers/front_back.hpp>

#include <containers/array/array.hpp>

namespace {

static_assert(containers::front(containers::array{5}) == 5);
static_assert(containers::back(containers::array{5}) == 5);

static_assert(containers::front(containers::array{2, 3, 4}) == 2);
static_assert(containers::back(containers::array{2, 3, 4}) == 4);

} // namespace