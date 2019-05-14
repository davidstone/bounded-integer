// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/begin_end.hpp>
#include <containers/push_back.hpp>
#include <containers/reserve_if_reservable.hpp>
#include <containers/size.hpp>

#include <bounded/integer.hpp>

namespace containers {
namespace detail {

template<typename, typename = void>
constexpr auto has_push_back = false;

template<typename Container>
constexpr auto has_push_back<
	Container,
	std::void_t<decltype(push_back(std::declval<Container &>(), std::declval<typename Container::value_type>()))>
> = true;

template<typename, typename = void>
constexpr auto has_size = false;

template<typename Container>
constexpr auto has_size<
	Container,
	std::void_t<decltype(size(std::declval<Container>()))>
> = true;

namespace common {

// TODO: work with non-sized containers and non-random-access containers
template<typename Container, typename Range, BOUNDED_REQUIRES(has_push_back<Container>)>
constexpr auto append(Container & output, Range && input) {
	auto const offset = size(output);
	if constexpr (has_size<Range>) {
		auto const input_size = [&] {
			auto const value = size(input);
			if constexpr (bounded::is_bounded_integer<decltype(value)>) {
				return value;
			} else {
				return typename Container::size_type(value);
			}
		};
		::containers::detail::growth_reallocation(output, input_size());
	}
	for (decltype(auto) value : BOUNDED_FORWARD(input)) {
		push_back(output, BOUNDED_FORWARD(value));
	}
	using traits = std::iterator_traits<typename Container::iterator>;
	return begin(output) + static_cast<typename traits::difference_type>(offset);
}

}	// namespace common

using ::containers::detail::common::append;

}	// namespace detail

using ::containers::detail::common::append;

}	// namespace containers
