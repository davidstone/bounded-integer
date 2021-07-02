// Copyright David Stone 2021.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/static_vector/static_vector.hpp>

#include <containers/is_range.hpp>
#include <containers/range_value_t.hpp>
#include <containers/size.hpp>

#include <numeric_traits/min_max_value.hpp>

namespace containers {

template<range Source>
constexpr auto make_static_vector(Source && source) {
	using value_type = range_value_t<Source>;
	constexpr auto size = static_cast<std::size_t>(numeric_traits::max_value<range_size_t<Source>>);
	return static_vector<value_type, size>(OPERATORS_FORWARD(source));
}

}	// namespace containers
