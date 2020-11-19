// Copyright David Stone 2016.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <containers/algorithms/reverse_iterator.hpp>

#include <containers/array/array.hpp>
#include <containers/algorithms/compare.hpp>
#include <containers/size.hpp>

namespace {

using namespace bounded::literal;

constexpr auto forward = containers::array<int, 5>{0, 1, 2, 3, 4};
constexpr auto reversed = containers::reversed(forward);

static_assert(containers::size(reversed) == 5_bi);
static_assert(begin(reversed) < end(reversed));
static_assert(begin(reversed) + 5_bi == end(reversed));

static_assert(containers::equal(containers::reversed(reversed), forward));

} // namespace