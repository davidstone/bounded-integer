// Copyright David Stone 2021.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <containers/iterator_t.hpp>

#include <containers/array/array.hpp>

namespace {

static_assert(
	std::is_same_v<containers::iterator_t<containers::array<int, 3> const &>,
	containers::contiguous_iterator<int const, 3>>
);
static_assert(std::is_same_v<
	containers::iterator_t<containers::array<int, 3> &>,
	containers::contiguous_iterator<int, 3>
>);
static_assert(std::is_same_v<
	containers::iterator_t<containers::array<int, 3> &&>,
	decltype(containers::move_iterator(std::declval<containers::contiguous_iterator<int, 3>>()))
>);

} // namespace