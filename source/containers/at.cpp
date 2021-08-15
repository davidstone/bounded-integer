// Copyright David Stone 2016.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <containers/at.hpp>

#include <containers/array/array.hpp>

namespace {

using namespace bounded::literal;

static_assert(containers::array({1, 2, 3})[0_bi] == 1);

} // namespace