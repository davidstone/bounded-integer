// Copyright David Stone 2021.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <containers/iter_difference_t.hpp>

#include <cstddef>

static_assert(std::is_same_v<containers::iter_difference_t<int *>, std::ptrdiff_t>);