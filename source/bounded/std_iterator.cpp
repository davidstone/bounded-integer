// Copyright David Stone 2020.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <bounded/std_iterator.hpp>

namespace {

static_assert(bounded::detail::std_random_access_iterator<std::vector<int>::iterator>);
static_assert(bounded::detail::std_bidirectional_iterator<std::vector<int>::iterator>);
static_assert(bounded::detail::std_iterator<std::vector<int>::iterator>);

static_assert(std::same_as<decltype(std::vector<int>::iterator() + bounded::constant<1>), std::vector<int>::iterator>);

} // namespace