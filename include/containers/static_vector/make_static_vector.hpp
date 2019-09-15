// Copyright David Stone 2015.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/static_vector/static_vector.hpp>

#include <limits>

namespace containers {

template<typename T, typename Size>
constexpr auto make_static_vector(Size const size) {
	return static_vector<T, static_cast<std::size_t>(std::numeric_limits<Size>::max())>(size);
}

}	// namespace containers
