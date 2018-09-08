// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/adapt.hpp>
#include <containers/algorithms/transform_iterator.hpp>

namespace containers {

template<typename Range, typename UnaryFunction>
constexpr auto transform(Range && range, UnaryFunction && dereference) {
	return adapt(BOUNDED_FORWARD(range), detail::transform_traits<UnaryFunction>(BOUNDED_FORWARD(dereference)));
}

}	// namespace containers
