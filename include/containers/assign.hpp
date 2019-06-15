// Copyright David Stone 2016.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/begin_end.hpp>
#include <containers/erase.hpp>
#include <containers/is_container.hpp>
#include <containers/is_iterator_sentinel.hpp>
#include <containers/repeat_n.hpp>

#include <bounded/integer.hpp>

#include <initializer_list>

namespace containers {
namespace detail {
namespace common {

// TODO: noexcept
template<typename Container, typename InputIterator, typename Sentinel> requires(
	is_container<Container> and is_iterator_sentinel<InputIterator, Sentinel>
)
constexpr auto assign(Container & container, InputIterator first, Sentinel const last) {
	// TODO: Do we try to reuse storage like this or just clear() + construct
	auto it = begin(container);
	for (; first != last; ++first) {
		if (it == end(container)) {
			break;
		}
		*it = *first;
		++it;
	}
	erase(container, it, end(container));
	for (; first != last; ++first) {
		container.emplace_back(*first);
	}
}
template<typename Container> requires is_container<Container>
constexpr auto assign(Container & container, std::initializer_list<typename Container::value_type> init) BOUNDED_NOEXCEPT(
	assign(container, begin(init), end(init))
)
template<typename Container, typename Size> requires(is_container<Container> and std::numeric_limits<Size>::is_integer)
constexpr auto assign(Container & container, Size const count, typename Container::value_type const & value) {
	auto const range = repeat_n(count, value);
	assign(container, begin(range), end(range));
}

}	// namespace common

using ::containers::detail::common::assign;

}	// namespace detail

using ::containers::detail::common::assign;

}	// namespace containers
