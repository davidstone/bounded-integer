// Copyright David Stone 2022.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <containers/map_value_type.hpp>

namespace {

static_assert(containers::get_key(containers::map_value_type{5, 2}) == 5);
static_assert(containers::get_mapped(containers::map_value_type{5, 2}) == 2);

static_assert(containers::get_key(std::pair{5, 2}) == 5);
static_assert(containers::get_mapped(std::pair{5, 2}) == 2);

} // namespace
