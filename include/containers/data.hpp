// Copyright David Stone 2016.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/begin_end.hpp>
#include <containers/is_iterable.hpp>

#include <bounded/integer.hpp>

namespace containers {
namespace detail {
namespace common {

template<typename Iterable, BOUNDED_REQUIRES(is_iterable<Iterable>)>
constexpr auto data(Iterable && iterable) BOUNDED_NOEXCEPT_DECLTYPE(
	pointer_from(begin(iterable))
)

}	// namespace common

using ::containers::detail::common::data;

}	// namespace detail

using ::containers::detail::common::data;

}	// namespace containers
