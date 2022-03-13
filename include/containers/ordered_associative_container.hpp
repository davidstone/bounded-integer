// Copyright David Stone 2020.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/associative_container.hpp>

#include <bounded/concepts.hpp>

namespace containers {
namespace detail {

template<typename T>
concept has_key_comp = requires(T const & map) {
	map.key_comp();
};

template<typename T>
concept has_extract_key = requires(T const & map) {
	map.extract_key();
};

} // namespace detail

template<typename T>
concept ordered_associative_range =
	associative_range<T> and
	(detail::has_key_comp<T> or detail::has_extract_key<T>);

template<typename T>
concept ordered_associative_container = ordered_associative_range<T> and associative_container<T>;

} // namespace containers
