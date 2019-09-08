// Copyright David Stone 2016.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/begin_end.hpp>
#include <containers/is_range.hpp>

#include <bounded/integer.hpp>

#include <type_traits>

namespace containers {

using namespace bounded::literal;

namespace detail {
namespace common {

template<typename Range>
concept never_empty_range = std::numeric_limits<typename std::remove_reference_t<Range>::size_type>::min() > 0_bi;

template<typename Range> requires range<Range>
constexpr auto empty(Range const & range) noexcept {
	// The never_empty check is not needed for correctness, but allows this
	// function to be constexpr in more situations.
	return never_empty_range<Range> ? false : begin(range) == end(range);
}

}	// namespace common

using ::containers::detail::common::empty;

}	// namespace detail

using ::containers::detail::common::empty;

}	// namespace containers
