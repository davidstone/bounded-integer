// Copyright David Stone 2016.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/begin_end.hpp>
#include <containers/common_iterator_functions.hpp>
#include <containers/is_range.hpp>
#include <containers/to_address.hpp>

#include <bounded/integer.hpp>

namespace containers {

constexpr auto data(range auto && r) {
	if constexpr (requires { r.data(); }) {
		return r.data();
	} else {
		return containers::to_address(containers::begin(r));
	}
}

} // namespace containers
