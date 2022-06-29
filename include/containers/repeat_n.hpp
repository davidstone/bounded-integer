// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/algorithms/generate.hpp>

#include <bounded/detail/construct_destroy.hpp>
#include <bounded/value_to_function.hpp>

namespace containers {

template<typename Size, typename T>
constexpr auto repeat_n(Size const size, T && value) {
	return generate_n(size, bounded::value_to_function(OPERATORS_FORWARD(value)));
}

template<typename T, typename Size>
constexpr auto repeat_default_n(Size const size) {
	return generate_n(size, bounded::construct<T>);
}

} // namespace containers
