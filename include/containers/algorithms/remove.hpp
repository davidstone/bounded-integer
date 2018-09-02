// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/algorithms/find.hpp>

#include <bounded/integer.hpp>

#include <utility>

namespace containers {

template<typename ForwardIterator, typename Sentinel, typename Predicate>
constexpr auto remove_if(ForwardIterator const first, Sentinel const last, Predicate predicate) {
	auto new_last = ::containers::find_if(first, last, predicate);
	if (new_last != last) {
		for (auto it = std::next(new_last); it != last; ++it) {
			if (!predicate(*it)) {
				*new_last = std::move(*it);
				++new_last;
			}
		}
	}
	return new_last;
}

template<typename Range, typename Predicate>
constexpr auto remove_if(Range & range, Predicate predicate) BOUNDED_NOEXCEPT_VALUE(
	::containers::remove_if(begin(range), end(range), BOUNDED_FORWARD(predicate))
)


template<typename ForwardIterator, typename Sentinel, typename T>
constexpr auto remove(ForwardIterator const first, Sentinel const last, T const & value) BOUNDED_NOEXCEPT(
	::containers::remove_if(first, last, bounded::equal_to(value))
)

template<typename Range, typename T>
constexpr auto remove(Range & range, T const & value) BOUNDED_NOEXCEPT_VALUE(
	::containers::remove_if(begin(range), end(range), value)
)

}	// namespace containers
