// Copyright David Stone 2016.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/begin_end.hpp>
#include <containers/is_iterable.hpp>

#include <bounded/integer.hpp>

#include <type_traits>

namespace containers {

using namespace bounded::literal;

namespace detail {
namespace common {

template<typename Iterable>
constexpr auto never_empty = std::numeric_limits<typename std::remove_reference_t<Iterable>::size_type>::min() > 0_bi;

template<typename Iterable, BOUNDED_REQUIRES(is_iterable<Iterable>)>
constexpr auto empty(Iterable const & iterable) noexcept {
	// The never_empty check is not needed for correctness, but allows this
	// function to be constexpr in more situations.
	return never_empty<Iterable> ? false : begin(iterable) == end(iterable);
}

}	// namespace common

using ::containers::detail::common::empty;

}	// namespace detail

using ::containers::detail::common::empty;

}	// namespace containers
