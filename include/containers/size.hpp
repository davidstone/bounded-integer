// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/algorithms/distance.hpp>
#include <containers/begin_end.hpp>
#include <containers/c_array.hpp>
#include <containers/is_range.hpp>

#include <bounded/integer.hpp>

namespace containers {

template<typename Range>
concept sized_range = (range<Range> and detail::has_member_size<Range>) or forward_random_access_range<Range>;

template<sized_range Range>
constexpr auto size(Range const & range) {
	if constexpr (detail::has_member_size<Range>) {
		return range.size();
	} else if constexpr (bounded::bounded_integer<typename Range::size_type>) {
		return typename Range::size_type(containers::end(range) - containers::begin(range), bounded::non_check);
	} else {
		return typename Range::size_type(containers::end(range) - containers::begin(range));
	}
}

template<typename T, std::size_t size_>
constexpr auto size(c_array<T, size_> const &) {
	return bounded::constant<size_>;
}

namespace detail {

template<range Range>
constexpr auto linear_size(Range const & r) {
	if constexpr (requires { containers::size(r); }) {
		return containers::size(r);
	} else {
		return static_cast<typename Range::size_type>(containers::distance(containers::begin(r), containers::end(r)));
	}
}

} // namespace detail
} // namespace containers