// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

module;

#include <operators/forward.hpp>
#include <operators/returns.hpp>

export module containers.algorithms.reverse_iterator;

import containers.algorithms.advance;
import containers.algorithms.compare;
import containers.adapt;
import containers.adapt_iterator;
import containers.array;
import containers.begin_end;
import containers.is_iterator;
import containers.random_access_iterator;
import containers.range;
import containers.size;

import bounded;
import std_module;

export import containers.common_iterator_functions;

namespace containers {

struct reverse_traits {
	static constexpr auto get_begin(auto && range) {
		return containers::end(OPERATORS_FORWARD(range));
	}

	static constexpr auto get_end(auto && range) {
		return containers::begin(OPERATORS_FORWARD(range));
	}

	static constexpr auto get_size(auto const & range) requires requires { containers::size(range); } {
		return containers::size(range);
	}
	template<typename Range> requires requires { Range::size(); }
	static constexpr auto get_size() {
		return Range::size();
	}

	static constexpr decltype(auto) dereference(bidirectional_iterator auto it) {
		return *::containers::prev(it);
	}

	static constexpr auto add(random_access_iterator auto it, auto const offset) OPERATORS_RETURNS(
		it - offset
	)

	static constexpr auto subtract(random_access_iterator auto const lhs, random_access_iterator auto const rhs) {
		return rhs - lhs;
	}

	static constexpr auto compare(random_access_iterator auto const lhs, random_access_iterator auto const rhs) {
		return rhs <=> lhs;
	}
	static constexpr auto equal(iterator auto const lhs, iterator auto const rhs) {
		return rhs == lhs;
	}
};

export constexpr auto reverse_iterator(bidirectional_iterator auto it) {
	return adapt_iterator(it, reverse_traits());
}

export constexpr auto reversed(range auto && source) {
	return adapt(OPERATORS_FORWARD(source), reverse_traits());
}

} // namespace containers

using namespace bounded::literal;

constexpr auto forward = containers::array({0, 1, 2, 3, 4});
constexpr auto reversed = containers::reversed(forward);

static_assert(containers::size(reversed) == 5_bi);
static_assert(begin(reversed) < end(reversed));
static_assert(begin(reversed) + 5_bi == end(reversed));

static_assert(containers::equal(containers::reversed(reversed), forward));

static_assert(*begin(reversed) == 4);
static_assert(*(begin(reversed) + 1_bi) == 3);
