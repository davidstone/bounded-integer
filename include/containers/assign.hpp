// Copyright David Stone 2016.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/append.hpp>
#include <containers/begin_end.hpp>
#include <containers/erase.hpp>
#include <containers/is_container.hpp>
#include <containers/is_range.hpp>
#include <containers/range_view.hpp>

#include <bounded/integer.hpp>

#include <initializer_list>

namespace containers {
namespace detail {
namespace common {

// TODO: noexcept
template<container Container, range Range>
constexpr auto assign(Container & container, Range && range) {
	auto it = begin(container);
	auto first = begin(BOUNDED_FORWARD(range));
	auto last = end(BOUNDED_FORWARD(range));
	for (; first != last; ++first) {
		if (it == end(container)) {
			break;
		}
		*it = *first;
		++it;
	}
	erase(container, it, end(container));
	append(container, range_view(first, last));
}
template<container Container>
constexpr auto assign(Container & container, std::initializer_list<typename Container::value_type> init) BOUNDED_NOEXCEPT(
	assign(container, range_view(init))
)

}	// namespace common

using ::containers::detail::common::assign;

}	// namespace detail

using ::containers::detail::common::assign;

}	// namespace containers
