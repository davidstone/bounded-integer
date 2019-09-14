// Copyright David Stone 2016.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/emplace_back.hpp>
#include <containers/is_container.hpp>
#include <containers/pop_back.hpp>
#include <containers/repeat_n.hpp>
#include <containers/size.hpp>
#include <containers/take.hpp>

#include <bounded/detail/forward.hpp>
#include <bounded/integer.hpp>

#include <utility>

namespace containers {
namespace detail {

template<typename Container, typename InitializerRange>
constexpr auto resize_impl(Container & container, InitializerRange const values) {
	while (size(container) > size(values)) {
		pop_back(container);
	}
	auto const remaining = size(values) - size(container);
	append(container, containers::take(values, remaining));
}

namespace common {

template<container Container, typename Size>
constexpr auto resize(Container & container, Size const count) {
	resize_impl(container, repeat_default_n<typename Container::value_type>(count));
}
template<container Container, typename Size>
constexpr auto resize(Container & container, Size const count, typename Container::value_type const & value) {
	resize_impl(container, repeat_n(count, value));
}

}	// namespace common

using ::containers::detail::common::resize;

}	// namespace detail

using ::containers::detail::common::resize;

}	// namespace containers
