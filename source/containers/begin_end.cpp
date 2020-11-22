// Copyright David Stone 2016.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <containers/begin_end.hpp>

namespace {

using namespace bounded::literal;

constexpr int a[] = {0, 1, 2, 3, 4};

static_assert(*containers::begin(a) == 0);
static_assert(containers::end(a) == containers::begin(a) + 5_bi);

} // namespace