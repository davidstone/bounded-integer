// Copyright David Stone 2015.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <containers/legacy_iterator.hpp>

#include <type_traits>

static_assert(std::is_convertible_v<containers::legacy_iterator<int const *>, containers::legacy_iterator<int const *>>);
static_assert(std::is_convertible_v<containers::legacy_iterator<int *>, containers::legacy_iterator<int *>>);
static_assert(std::is_convertible_v<containers::legacy_iterator<int *>, containers::legacy_iterator<int const *>>);
static_assert(!std::is_convertible_v<containers::legacy_iterator<int const *>, containers::legacy_iterator<int *>>);
