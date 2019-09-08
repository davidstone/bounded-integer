// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/begin_end.hpp>
#include <bounded/detail/forward.hpp>
#include <utility>

namespace containers {
namespace detail {

template<typename Range>
concept noexcept_iterable =
	noexcept(begin(std::declval<Range>()) != end(std::declval<Range>())) and
	noexcept(++std::declval<decltype(begin(std::declval<Range>())) &>());

}	// namespace detail
}	// namespcae containers
