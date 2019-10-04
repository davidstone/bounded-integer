// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/begin_end.hpp>
#include <containers/range_view.hpp>

#include <bounded/integer.hpp>

#include <iterator>
#include <limits>

namespace containers {

using namespace bounded::literal;

template<typename Range, typename Predicate>
constexpr auto count_if(Range && range, Predicate predicate) {
	constexpr auto maximum = bounded::max_value<decltype(size(range))>;
	bounded::integer<0, bounded::detail::normalize<maximum.value()>> sum = 0_bi;
	for (decltype(auto) value : BOUNDED_FORWARD(range)) {
		if (predicate(BOUNDED_FORWARD(value))) {
			++sum;
		}
	}
	return sum;
}

template<typename Iterator, typename Sentinel, typename Predicate>
constexpr auto count_if(Iterator const first, Sentinel const last, Predicate predicate) {
	return ::containers::count_if(range_view(first, last), std::move(predicate));
}

template<typename Range, typename T>
constexpr auto count(Range && range, T const & value) {
	return ::containers::count_if(BOUNDED_FORWARD(range), bounded::equal_to(value));
}

template<typename Iterator, typename Sentinel, typename T>
constexpr auto count(Iterator const first, Sentinel const last, T const & value) {
	return ::containers::count_if(range_view(first, last), bounded::equal_to(value));
}

}	// namespace containers
