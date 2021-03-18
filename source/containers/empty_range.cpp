// Copyright David Stone 2021.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <containers/empty_range.hpp>

namespace {

constexpr auto range = containers::empty_range<int>();

static_assert(range.begin() == range.end());

static_assert(requires(containers::detail::empty_iterator<int> it) {
	++it;
	it - it;
	it + bounded::constant<0>;
});

} // namespace