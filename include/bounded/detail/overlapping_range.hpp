// Copyright David Stone 2017.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <bounded/detail/builtin_min_max_value.hpp>
#include <bounded/detail/comparison.hpp>

namespace bounded {
namespace detail {

template<typename T, typename Value>
constexpr auto value_fits_in_type(Value const value) {
	return safe_compare(builtin_min_value<T>, value) <= 0 and safe_compare(value, builtin_max_value<T>) <= 0;
}

}	// namespace detail
}	// namespace bounded
