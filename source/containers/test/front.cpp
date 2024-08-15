// Copyright David Stone 2024.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

export module containers.test.front;

import containers.array;
import containers.front;

static_assert(containers::front(containers::array({5})) == 5);
static_assert(containers::front(containers::array({2, 3, 4})) == 2);