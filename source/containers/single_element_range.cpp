// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <containers/single_element_range.hpp>

static_assert(!containers::detail::single_element_iterator<char const &>('a').is_end());
static_assert((containers::detail::single_element_iterator<char const &>('a') + bounded::constant<1>).is_end());
