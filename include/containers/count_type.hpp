// Copyright David Stone 2021.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/iterator_t.hpp>
#include <containers/offset_type.hpp>

namespace containers {

template<typename Range>
using count_type = offset_type<iterator_t<Range &>>;

}	// namespace containers
