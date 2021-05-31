// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <containers/range_view.hpp>

#include <containers/array/array.hpp>
#include <containers/algorithms/compare.hpp>

namespace {

constexpr auto a = containers::array({0, 1, 2, 3, 4});
static_assert(containers::equal(a, containers::range_view(a)));

} // namespace