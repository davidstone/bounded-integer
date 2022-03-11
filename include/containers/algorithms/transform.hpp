// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/algorithms/transform_iterator.hpp>
#include <containers/adapt.hpp>
#include <containers/default_begin_end_size.hpp>
#include <containers/is_range.hpp>

namespace containers {
namespace detail {

template<typename UnaryFunction>
struct range_transform_traits : transform_traits<UnaryFunction>, default_begin_end_size {
	using transform_traits<UnaryFunction>::transform_traits;
};

template<typename UnaryFunction>
range_transform_traits(UnaryFunction) -> range_transform_traits<UnaryFunction>;

template<typename UnaryFunction>
struct range_transform_traits_dereference : transform_traits_dereference<UnaryFunction>, default_begin_end_size {
	using transform_traits_dereference<UnaryFunction>::transform_traits_dereference;
};

template<typename UnaryFunction>
range_transform_traits_dereference(UnaryFunction) -> range_transform_traits_dereference<UnaryFunction>;

} // namespace detail

constexpr auto transform(range auto && source, auto dereference) {
	return adapt(OPERATORS_FORWARD(source), detail::range_transform_traits(std::move(dereference)));
}

constexpr auto transform_dereference(range auto && source, auto dereference) {
	return adapt(OPERATORS_FORWARD(source), detail::range_transform_traits_dereference(std::move(dereference)));
}

}	// namespace containers
