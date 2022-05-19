// Copyright David Stone 2021.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/is_range.hpp>
#include <containers/size.hpp>

namespace containers::detail {

template<typename Container>
concept can_set_size =
	range<Container> and
	requires(Container & container, range_size_t<Container> const new_size) {
		container.set_size(new_size);
	};

} // namespace containers::detail