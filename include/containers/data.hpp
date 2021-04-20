// Copyright David Stone 2016.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/begin_end.hpp>
#include <containers/is_range.hpp>
#include <containers/iterator_t.hpp>
#include <containers/to_address.hpp>

#include <bounded/integer.hpp>

namespace containers {
namespace detail {

template<typename Range>
concept has_member_data = requires(Range r) { r.data(); };

template<typename Iterator, typename Sentinel>
concept contiguous_iterator_sentinel = to_addressable<Iterator> and random_access_sentinel_for<Sentinel, Iterator>;

} // namespace detail

template<typename T>
concept contiguous_range =
	random_access_range<T> and
	(detail::has_member_data<T> or detail::contiguous_iterator_sentinel<iterator_t<T>, detail::sentinel_t<T>>);

template<contiguous_range Range>
constexpr auto data(Range && r) {
	if constexpr (detail::has_member_data<Range>) {
		return r.data();
	} else {
		return containers::to_address(containers::begin(r));
	}
}

} // namespace containers
