// Copyright David Stone 2016.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/begin_end.hpp>
#include <containers/is_iterable.hpp>
#include <containers/is_iterator.hpp>

#include <bounded/integer.hpp>

namespace containers {
namespace detail {

template<typename Iterable, typename Iterator, BOUNDED_REQUIRES(is_iterable<Iterable> and is_iterator<Iterator>)>
constexpr auto mutable_iterator(Iterable & iterable, Iterator const it) BOUNDED_NOEXCEPT_VALUE(
	begin(iterable) + (it - begin(iterable))
)

}	// namespace detail
}	// namespace containers
