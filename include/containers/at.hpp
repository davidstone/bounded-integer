// Copyright David Stone 2016.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/index_type.hpp>
#include <containers/is_range.hpp>
#include <containers/size.hpp>

#include <bounded/detail/forward.hpp>
#include <bounded/integer.hpp>

#include <utility>

namespace containers {

using namespace bounded::literal;

namespace detail {
namespace common {

template<typename Range, typename Index> requires is_range<Range>
constexpr decltype(auto) at(Range && range, Index const index) BOUNDED_NOEXCEPT(
	BOUNDED_FORWARD(range)[typename index_type<Range>::overflow_policy{}.assignment(
		bounded::integer(index),
		0_bi,
		size(range) - 1_bi
	)]
)

template<typename Range, typename Index> requires is_range<Range>
constexpr decltype(auto) at(Range && range, Index const index, bounded::non_check_t) BOUNDED_NOEXCEPT(
	BOUNDED_FORWARD(range)[static_cast<index_type<Range>>(index)]
)


}	// namespace common

using ::containers::detail::common::at;

}	// namespace detail

using ::containers::detail::common::at;

}	// namespace containers
