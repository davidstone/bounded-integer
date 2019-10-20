// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/integer_range.hpp>
#include <containers/is_iterator.hpp>
#include <bounded/integer.hpp>

namespace containers {

constexpr auto copy_n(iterator auto first, auto const count, iterator auto out) {
	using namespace bounded::literal;
	static_assert(count > -1_bi, "Negative numbers not supported.");
	struct result {
		decltype(first) input;
		decltype(out) output;
	};
	for (auto const n [[maybe_unused]] : integer_range(count)) {
		*out = *first;
		++out;
		++first;
	}
	return result{std::move(first), std::move(out)};
}

}	// namespace containers
