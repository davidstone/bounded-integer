// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/begin_end.hpp>
#include <containers/noexcept_iterable.hpp>
#include <containers/range_view.hpp>

#include <bounded/integer.hpp>

#include <iterator>
#include <limits>

namespace containers {

using namespace bounded::literal;

template<typename Range, typename Predicate>
constexpr auto count_if(Range && range, Predicate predicate)
	noexcept(detail::noexcept_iterable<Range> and noexcept(predicate(*begin(BOUNDED_FORWARD(range)))))
{
	constexpr auto maximum = std::numeric_limits<decltype(size(range))>::max();
	bounded::integer<0, bounded::detail::normalize<maximum.value()>> sum = 0_bi;
	for (decltype(auto) value : BOUNDED_FORWARD(range)) {
		if (predicate(BOUNDED_FORWARD(value))) {
			++sum;
		}
	}
	return sum;
}

template<typename Iterator, typename Sentinel, typename Predicate>
constexpr auto count_if(Iterator const first, Sentinel const last, Predicate predicate) BOUNDED_NOEXCEPT_VALUE(
	::containers::count_if(range_view(first, last), std::move(predicate))
);

template<typename Range, typename T>
constexpr auto count(Range && range, T const & value) BOUNDED_NOEXCEPT_VALUE(
	::containers::count_if(BOUNDED_FORWARD(range), bounded::equal_to(value))
)

template<typename Iterator, typename Sentinel, typename T>
constexpr auto count(Iterator const first, Sentinel const last, T const & value) BOUNDED_NOEXCEPT_VALUE(
	::containers::count_if(range_view(first, last), bounded::equal_to(value))
)

}	// namespace containers
