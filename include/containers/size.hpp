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

// Ignored... makes this a worse match than std::size if that compiles
template<typename Iterable, typename... Ignored, BOUNDED_REQUIRES(is_iterable<Iterable> and bounded::is_bounded_integer<typename Iterable::size_type>)>
constexpr auto size(Iterable const & iterable, Ignored && ...) noexcept {
	return typename Iterable::size_type(end(iterable) - begin(iterable), bounded::non_check);
}
template<typename Iterable, typename... Ignored, BOUNDED_REQUIRES(is_iterable<Iterable> and !bounded::is_bounded_integer<typename Iterable::size_type>)>
constexpr auto size(Iterable const & iterable, Ignored && ...) noexcept {
	return typename Iterable::size_type(end(iterable) - begin(iterable));
}

}	// namespace common

using ::containers::detail::common::size;

}	// namespace detail

using ::containers::detail::common::size;

}	// namespace containers
