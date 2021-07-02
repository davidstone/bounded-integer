// Copyright David Stone 2021.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <containers/static_vector/make_static_vector.hpp>

#include <containers/array/array.hpp>

#include "../../bounded/homogeneous_equals.hpp"

namespace {

static_assert(homogeneous_equals(
	containers::make_static_vector(containers::array({5, 3})),
	containers::static_vector<int, 2>({5, 3})
));

} // namespace
