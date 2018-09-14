// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/begin_end.hpp>
#include <containers/is_iterator_sentinel.hpp>
#include <containers/push_back.hpp>
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

namespace common {

// TODO: specialize for ForwardIterator to call reserve
// TODO: work with non-sized containers
template<typename Container, typename InputIterator, typename Sentinel, BOUNDED_REQUIRES(has_push_back<Container> and is_iterator_sentinel<InputIterator, Sentinel>)>
constexpr auto append(Container & container, InputIterator first, Sentinel last) {
	auto const offset = size(container);
	for (; first != last; ++first) {
		push_back(container, *first);
	}
	using traits = std::iterator_traits<typename Container::iterator>;
	return begin(container) + static_cast<typename traits::difference_type>(offset);
}

}	// namespace common

using ::containers::detail::common::append;

}	// namespace detail

using ::containers::detail::common::append;

}	// namespace containers
