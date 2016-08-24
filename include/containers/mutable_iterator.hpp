// Copyright David Stone 2016.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/begin_end.hpp>
#include <containers/is_container.hpp>
#include <containers/is_iterator.hpp>

#include <bounded/integer.hpp>

namespace containers {
namespace detail {

template<typename Container, typename Iterator, BOUNDED_REQUIRES(is_container<Container> and is_iterator<Iterator>)>
constexpr auto mutable_iterator(Container & container, Iterator const it) BOUNDED_NOEXCEPT_VALUE(
	begin(container) + (it - begin(container))
)

}	// namespace detail
}	// namespace containers
