// Copyright David Stone 2021.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <containers/offset_type.hpp>

#include <containers/contiguous_iterator.hpp>

#include <cstddef>
#include <type_traits>

static_assert(std::is_same_v<containers::offset_type<int *>, std::ptrdiff_t>);
static_assert(std::is_same_v<containers::offset_type<containers::contiguous_iterator<int, 5>>, bounded::integer<0, 5>>);