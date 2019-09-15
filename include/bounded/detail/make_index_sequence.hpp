// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <bounded/detail/class.hpp>

#include <type_traits>

namespace bounded {

template<auto n>
constexpr auto make_index_sequence(constant_t<n>) {
	return std::make_index_sequence<n>{};
}

}	// namespace bounded
