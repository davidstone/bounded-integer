// Copyright David Stone 2021.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <containers/iterator_t.hpp>

#include <containers/array.hpp>

namespace {

using namespace bounded::literal;

constexpr auto size = 3_bi;

static_assert(std::same_as<
	containers::iterator_t<containers::array<int, size> const &>,
	containers::contiguous_iterator<int const, 3>
>);
static_assert(std::same_as<
	containers::iterator_t<containers::array<int, size> &>,
	containers::contiguous_iterator<int, 3>
>);
static_assert(std::same_as<
	containers::iterator_t<containers::array<int, size> &&>,
	containers::contiguous_iterator<int, 3>
>);

} // namespace