// Copyright David Stone 2017.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <bounded/integer.hpp>

#include <type_traits>

namespace containers {
namespace detail {

template<auto n>
constexpr auto make_index_sequence(bounded::constant_t<n>) noexcept {
	return std::make_index_sequence<n>{};
}

}	// namespace detail
}	// namespace containers
