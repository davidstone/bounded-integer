// Copyright David Stone 2015.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <iterator>

namespace containers {

template<typename Iterator>
concept iterator = requires { typename std::iterator_traits<Iterator>::iterator_category; };

}	// namespace containers
