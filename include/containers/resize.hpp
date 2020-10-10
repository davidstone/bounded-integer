// Copyright David Stone 2016.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/append.hpp>
#include <containers/pop_back.hpp>
#include <containers/repeat_n.hpp>
#include <containers/resizable_container.hpp>
#include <containers/size.hpp>
#include <containers/take.hpp>

#include <operators/forward.hpp>
#include <bounded/integer.hpp>

#include <utility>

namespace containers {
namespace detail {

constexpr auto resize_impl(auto & container_to_resize, auto const initializer_range) {
	while (size(container_to_resize) > size(initializer_range)) {
		pop_back(container_to_resize);
	}
	auto const remaining = size(initializer_range) - size(container_to_resize);
	append(container_to_resize, containers::take(initializer_range, remaining));
}

namespace common {

template<resizable_container Container>
constexpr auto resize(Container & container_to_resize, auto const count) {
	resize_impl(container_to_resize, repeat_default_n<typename Container::value_type>(count));
}
template<resizable_container Container>
constexpr auto resize(Container & container_to_resize, auto const count, typename Container::value_type const & value) {
	resize_impl(container_to_resize, repeat_n(count, value));
}

}	// namespace common

using ::containers::detail::common::resize;

}	// namespace detail

using ::containers::detail::common::resize;

}	// namespace containers
