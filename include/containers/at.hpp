// Copyright David Stone 2016.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/index_type.hpp>
#include <containers/is_range.hpp>
#include <containers/size.hpp>

#include <operators/forward.hpp>
#include <bounded/integer.hpp>

#include <utility>

namespace containers {

using namespace bounded::literal;

namespace detail {
namespace common {

constexpr decltype(auto) at(range auto && r, auto const index) {
	return OPERATORS_FORWARD(r)[bounded::throw_policy<std::out_of_range>::assignment(
		bounded::integer(index),
		0_bi,
		size(r) - 1_bi
	)];
}

constexpr decltype(auto) at(range auto && r, auto const index, bounded::non_check_t) {
	return OPERATORS_FORWARD(r)[static_cast<index_type<decltype(r)>>(index)];
}


}	// namespace common

using ::containers::detail::common::at;

}	// namespace detail

using ::containers::detail::common::at;

}	// namespace containers
