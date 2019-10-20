// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/algorithms/find.hpp>
#include <containers/is_iterator_sentinel.hpp>
#include <containers/is_range.hpp>

#include <bounded/integer.hpp>
#include <bounded/optional.hpp>

#include <type_traits>
#include <utility>

namespace containers {

// the maybe_find functions return a value contextually convertible to bool. If
// the range contains the element, it can be dereferenced as an optional
template<iterator ForwardIterator>
constexpr auto maybe_find_if(ForwardIterator const first, sentinel_for<ForwardIterator> auto const last, auto predicate) {
	auto const it = ::containers::find_if(first, last, std::move(predicate));
	if constexpr (std::is_lvalue_reference_v<decltype(*it)>) {
		return it != last ? std::addressof(*it) : nullptr;
	} else {
		return it != last ? bounded::make_optional(*it) : bounded::none;
	}
}

constexpr auto maybe_find_if(range auto && range, auto predicate) {
	return ::containers::maybe_find_if(begin(BOUNDED_FORWARD(range)), end(BOUNDED_FORWARD(range)), std::move(predicate));
}


template<iterator ForwardIterator>
constexpr auto maybe_find(ForwardIterator const first, sentinel_for<ForwardIterator> auto const last, auto const & value) {
	return ::containers::maybe_find_if(first, last, bounded::equal_to(value));
}

constexpr auto maybe_find(range auto && range, auto const & value) {
	return ::containers::maybe_find(begin(BOUNDED_FORWARD(range)), end(BOUNDED_FORWARD(range)), value);
}

}	// namespace containers
