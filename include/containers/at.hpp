// Copyright David Stone 2016.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/index_type.hpp>
#include <containers/is_range.hpp>
#include <containers/size.hpp>

#include <operators/forward.hpp>
#include <bounded/check_in_range.hpp>
#include <bounded/integer.hpp>

#include <stdexcept>

namespace containers {

using namespace bounded::literal;

constexpr decltype(auto) at(range auto && r, auto const index) {
	return OPERATORS_FORWARD(r)[bounded::check_in_range<std::out_of_range>(
		bounded::integer(index),
		0_bi,
		containers::size(r) - 1_bi
	)];
}

constexpr decltype(auto) at(range auto && r, auto const index, bounded::non_check_t) {
	using index_t = index_type<decltype(r)>;
	auto const converted = index_t(index);
	return OPERATORS_FORWARD(r)[converted];
}

} // namespace containers
