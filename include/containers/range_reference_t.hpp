// Copyright David Stone 2022.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/is_range.hpp>
#include <containers/iter_reference_t.hpp>
#include <containers/iterator_t.hpp>

namespace containers {

template<range Range>
using range_reference_t = iter_reference_t<iterator_t<Range>>;

} // namespace containers
