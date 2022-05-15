// Copyright David Stone 2016.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/algorithms/erase.hpp>
#include <containers/algorithms/generate.hpp>
#include <containers/append.hpp>
#include <containers/count_type.hpp>
#include <containers/pop_back.hpp>
#include <containers/range_value_t.hpp>
#include <containers/repeat_n.hpp>
#include <containers/resizable_container.hpp>
#include <containers/size.hpp>

#include <operators/forward.hpp>
#include <bounded/detail/cast.hpp>
#include <bounded/integer.hpp>

#include <utility>

namespace containers {
namespace detail {

template<typename Container>
constexpr auto resize_impl(Container & container_to_resize, auto const new_size, auto const generator) {
	auto const container_size = bounded::integer(containers::size(container_to_resize));
	if (new_size <= container_size) {
		containers::erase_after(container_to_resize, containers::begin(container_to_resize) + ::bounded::assume_in_range<containers::count_type<Container>>(new_size));
	} else {
		auto const remaining = bounded::increase_min<0>(new_size - container_size, bounded::unchecked);
		::containers::append(container_to_resize, containers::generate_n(remaining, generator));
	}
}

} // namespace detail

template<resizable_container Container>
constexpr auto resize(Container & container_to_resize, auto const new_size) {
	::containers::detail::resize_impl(container_to_resize, bounded::integer(new_size), bounded::construct_return<range_value_t<Container>>);
}
template<resizable_container Container>
constexpr auto resize(Container & container_to_resize, auto const new_size, range_value_t<Container> const & value) {
	::containers::detail::resize_impl(container_to_resize, bounded::integer(new_size), bounded::value_to_function(value));
}

} // namespace containers
