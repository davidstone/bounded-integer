// Copyright David Stone 2015.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <containers/legacy_iterator.hpp>

#include <containers/contiguous_iterator.hpp>

#include <type_traits>

static_assert(std::is_convertible_v<containers::legacy_iterator<int const *>, containers::legacy_iterator<int const *>>);
static_assert(std::is_convertible_v<containers::legacy_iterator<int *>, containers::legacy_iterator<int *>>);
static_assert(std::is_convertible_v<containers::legacy_iterator<int *>, containers::legacy_iterator<int const *>>);
static_assert(!std::is_convertible_v<containers::legacy_iterator<int const *>, containers::legacy_iterator<int *>>);

template<typename T>
concept incrementable = requires(T value) { ++value; };

static_assert(incrementable<containers::legacy_iterator<int *>>);
static_assert(incrementable<containers::legacy_iterator<containers::contiguous_iterator<int, 1>>>);
static_assert(incrementable<containers::contiguous_iterator<int, 0>>);
static_assert(incrementable<containers::legacy_iterator<containers::contiguous_iterator<int, 0>>>);
