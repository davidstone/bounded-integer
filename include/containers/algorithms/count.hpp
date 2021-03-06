// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/count_type.hpp>
#include <containers/range_view.hpp>
#include <containers/size.hpp>

#include <bounded/integer.hpp>

#include <iterator>
#include <limits>

namespace containers {

using namespace bounded::literal;

template<range Range>
constexpr auto count_if(Range && r, auto predicate) {
	auto sum = count_type<Range>(0_bi);
	for (decltype(auto) value : OPERATORS_FORWARD(r)) {
		if (predicate(OPERATORS_FORWARD(value))) {
			++sum;
		}
	}
	return sum;
}

template<iterator Iterator>
constexpr auto count_if(Iterator const first, sentinel_for<Iterator> auto const last, auto predicate) {
	return ::containers::count_if(range_view(first, last), std::move(predicate));
}

constexpr auto count(range auto && range, auto const & value) {
	return ::containers::count_if(OPERATORS_FORWARD(range), bounded::equal_to(value));
}

template<iterator Iterator>
constexpr auto count(Iterator const first, sentinel_for<Iterator> auto const last, auto const & value) {
	return ::containers::count_if(range_view(first, last), bounded::equal_to(value));
}

}	// namespace containers
