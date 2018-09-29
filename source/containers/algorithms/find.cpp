// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <containers/algorithms/find.hpp>

#include <containers/array/array.hpp>

namespace {
using namespace bounded::literal;

constexpr auto data = containers::array{1, 2, 5, 9, 0};

static_assert(containers::maybe_find(data, 5) == std::addressof(data[2_bi]));
static_assert(!containers::maybe_find(data, 7));
static_assert(containers::maybe_find_if(data, [](auto const x) { return x % 2 == 0; }) == std::addressof(data[1_bi]));

} // namespace
