// Copyright David Stone 2016.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/begin_end.hpp>
#include <containers/is_container.hpp>
#include <containers/is_iterator_sentinel.hpp>
#include <containers/size.hpp>

#include <bounded/integer.hpp>

namespace containers {
namespace detail {
namespace common {

// TODO: specialize for ForwardIterator to call reserve
template<typename Container, typename InputIterator, typename Sentinel, BOUNDED_REQUIRES(is_container<Container> and is_iterator_sentinel<InputIterator, Sentinel>)>
constexpr auto append(Container & container, InputIterator first, Sentinel last) {
	auto const offset = size(container);
	for (; first != last; ++first) {
		container.emplace_back(*first);
	}
	return begin(container) + offset;
}

}	// namespace common
}	// namespace detail
}	// namespace containers
