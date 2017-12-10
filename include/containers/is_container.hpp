// Copyright David Stone 2015.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/has_nested_type.hpp>

namespace containers {
namespace detail {

CONTAINERS_MAKE_NESTED_TYPE_TEST(iterator)
CONTAINERS_MAKE_NESTED_TYPE_TEST(const_iterator)

}	// namespace detail

template<typename Container>
constexpr auto is_container = detail::has_nested_type_iterator<Container> or detail::has_nested_type_const_iterator<Container>;

}	// namespace containers
