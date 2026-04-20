// Copyright David Stone 2020.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

module;

#include <bounded/assert.hpp>

export module containers.test.lookup;

import containers.algorithms.find;
import containers.array;
import containers.begin_end;
import containers.lookup;
import containers.map_value_type;
import containers.size;

import bounded;

using namespace bounded::literal;

struct container {
	using key_type = int;
	using mapped_type = int;

	constexpr auto begin() const {
		return containers::begin(m_data);
	}
	constexpr auto size() const {
		return containers::size(m_data);
	}
	constexpr auto find(int const x) const {
		return containers::find_if(m_data, [=](containers::map_value_type<int, int> const value) { return value.key == x; });
	}

	containers::array<containers::map_value_type<int, int>, 2_bi> m_data;
};

static_assert([]{
	auto const map = container({1, 3});
	BOUNDED_ASSERT(*containers::lookup(map, 1) == 3);
	BOUNDED_ASSERT(!containers::lookup(map, 2));
	return true;
}());