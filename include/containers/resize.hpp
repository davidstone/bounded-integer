// Copyright David Stone 2016.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/is_container.hpp>
#include <containers/size.hpp>

#include <bounded/integer.hpp>

#include <utility>

namespace containers {
namespace detail {

struct common_resize_tag{};
template<typename Container, typename Size, typename... MaybeInitializer>
constexpr auto resize(common_resize_tag, Container & container, Size const count, MaybeInitializer && ... args) {
	static_assert(sizeof...(MaybeInitializer) == 0 or sizeof...(MaybeInitializer) == 1);
	while (size(container) > count) {
		container.pop_back();
	}
	while (size(container) < count) {
		container.emplace_back(std::forward<MaybeInitializer>(args)...);
	}
}

namespace common {

template<typename Container, typename Size, BOUNDED_REQUIRES(is_container<Container>)>
constexpr auto resize(Container & container, Size const count) BOUNDED_NOEXCEPT(
	resize(common_resize_tag{}, container, count)
)
template<typename Container, typename Size, BOUNDED_REQUIRES(is_container<Container>)>
constexpr auto resize(Container & container, Size const count, typename Container::value_type const & value) BOUNDED_NOEXCEPT(
	resize(common_resize_tag{}, container, count, value)
)

}	// namespace common

using ::containers::detail::common::resize;

}	// namespace detail

using ::containers::detail::common::resize;

}	// namespace containers
