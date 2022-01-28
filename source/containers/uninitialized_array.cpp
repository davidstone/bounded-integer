// Copyright David Stone 2021.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <containers/uninitialized_array.hpp>

namespace {

using namespace bounded::literal;
using container = containers::uninitialized_array<int, 5_bi>;

static_assert(container().data() != container().data());

} // namespace