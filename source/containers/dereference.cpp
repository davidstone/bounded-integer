// Copyright David Stone 2021.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <containers/dereference.hpp>

#include <containers/array.hpp>

namespace {

using namespace bounded::literal;
constexpr auto size = 0_bi;

static_assert(std::is_same_v<
	decltype(containers::dereference<containers::array<int, size>>({})),
	int &&
>);
static_assert(std::is_same_v<
	decltype(containers::dereference<containers::array<int, size> const &>({})),
	int const &
>);
static_assert(std::is_same_v<
	decltype(containers::dereference<containers::array<int, size> &>({})),
	int &
>);

} // namespace