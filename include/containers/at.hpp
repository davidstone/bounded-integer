// Copyright David Stone 2016.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/index_type.hpp>
#include <containers/is_iterable.hpp>
#include <containers/size.hpp>

#include <bounded/integer.hpp>

#include <utility>

namespace containers {

using namespace bounded::literal;

namespace detail {
namespace common {

template<typename Iterable, typename Index, BOUNDED_REQUIRES(is_iterable<Iterable>)>
constexpr decltype(auto) at(Iterable && iterable, Index const index) BOUNDED_NOEXCEPT(
	std::forward<Iterable>(iterable)[typename index_type<Iterable>::overflow_policy{}.assignment(
		bounded::make(index),
		0_bi,
		size(iterable) - 1_bi
	)]
)

template<typename Iterable, typename Index, BOUNDED_REQUIRES(is_iterable<Iterable>)>
constexpr decltype(auto) at(Iterable && iterable, Index const index, bounded::non_check_t) BOUNDED_NOEXCEPT(
	std::forward<Iterable>(iterable)[static_cast<index_type<Iterable>>(index)]
)


}	// namespace common

using ::containers::detail::common::at;

}	// namespace detail

using ::containers::detail::common::at;

}	// namespace containers
