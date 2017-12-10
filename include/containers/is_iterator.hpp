// Copyright David Stone 2015.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/has_nested_type.hpp>

#include <iterator>

namespace containers {
namespace detail {

CONTAINERS_MAKE_NESTED_TYPE_TEST(iterator_category)

}	// namespace detail

template<typename Iterator>
constexpr auto is_iterator = detail::has_nested_type_iterator_category<std::iterator_traits<Iterator>>;

}	// namespace containers
