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

struct map_t {
	using key_type = int;
	using mapped_type = int;

	constexpr auto begin() const {
		return std::addressof(m_value);
	}
	static constexpr auto size() {
		return 1_bi;
	}
	constexpr auto find(int const x) const {
		return m_value.key == x ? begin() : begin() + size();
	}

	containers::map_value_type<int, int> m_value;
};

static_assert([]{
	auto const map = map_t({1, 3});
	BOUNDED_ASSERT(*containers::lookup(map, 1) == 3);
	BOUNDED_ASSERT(*containers::lookup_value(map, 1) == containers::map_value_type{1, 3});
	BOUNDED_ASSERT(!containers::lookup(map, 2));
	BOUNDED_ASSERT(!containers::lookup_value(map, 2));
	return true;
}());

struct set_t {
	using key_type = int;

	constexpr auto begin() const {
		return std::addressof(m_value);
	}
	static constexpr auto size() {
		return 1_bi;
	}
	constexpr auto find(int const x) const {
		return m_value == x ? begin() : begin() + size();
	}

	int m_value;
};

static_assert([]{
	auto const set = set_t({1});
	BOUNDED_ASSERT(*containers::lookup_value(set, 1) == 1);
	BOUNDED_ASSERT(!containers::lookup_value(set, 2));
	return true;
}());
