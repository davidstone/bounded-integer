// Copyright David Stone 2021.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/is_range.hpp>
#include <containers/iter_difference_t.hpp>
#include <containers/iterator_t.hpp>

namespace containers::detail {

template<typename Container>
concept appendable_from_capacity =
	range<Container> and
	requires(Container & container, iter_difference_t<iterator_t<Container>> const difference) {
		container.append_from_capacity(difference);
	};

} // namespace containers::detail