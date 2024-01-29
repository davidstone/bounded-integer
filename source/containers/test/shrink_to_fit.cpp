// Copyright David Stone 2020.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

module;

#include <bounded/assert.hpp>

export module containers.test.shrink_to_fit;

import containers.algorithms.compare;
import containers.begin_end;
import containers.c_array;
import containers.shrink_to_fit;
import containers.size;
import containers.vector;

import bounded;
import bounded.test_int;
import std_module;

using namespace bounded::literal;

namespace {

struct container {
	constexpr container() {
		m_data.reserve(1_bi);
	}
	template<std::size_t size>
	constexpr container(containers::c_array<bounded_test::non_copyable_integer, size> && init):
		m_data(std::move(init))
	{
	}

	constexpr auto begin() const {
		return containers::begin(m_data);
	}
	constexpr auto begin() {
		return containers::begin(m_data);
	}
	constexpr auto size() const {
		return containers::size(m_data);
	}

	constexpr void reserve(auto const new_capacity) & {
		m_data.reserve(new_capacity);
	}
	constexpr auto capacity() const {
		return bounded::assume_in_range(m_data.capacity(), 1_bi, 5_bi);
	}
	constexpr void set_size(auto const new_size) {
		m_data.set_size(new_size);
	}

private:
	containers::vector<bounded_test::non_copyable_integer> m_data;
};

static_assert(container().capacity() == 1_bi);

static_assert([]{
	auto x = container();
	containers::shrink_to_fit(x);
	BOUNDED_ASSERT(x.capacity() == 1_bi);
	BOUNDED_ASSERT(containers::equal(x, containers::vector<bounded_test::non_copyable_integer>()));
	return true;
}());

static_assert([]{
	auto x = container({5});
	containers::shrink_to_fit(x);
	BOUNDED_ASSERT(x.capacity() == 1_bi);
	BOUNDED_ASSERT(containers::equal(x, containers::vector({bounded_test::non_copyable_integer(5)})));
	return true;
}());

static_assert([]{
	auto x = container();
	x.reserve(4_bi);
	containers::shrink_to_fit(x);
	BOUNDED_ASSERT(x.capacity() == 1_bi);
	BOUNDED_ASSERT(containers::equal(x, containers::vector<bounded_test::non_copyable_integer>()));
	return true;
}());

static_assert([]{
	auto x = container({5});
	x.reserve(4_bi);
	containers::shrink_to_fit(x);
	BOUNDED_ASSERT(x.capacity() == 1_bi);
	BOUNDED_ASSERT(containers::equal(x, containers::vector({bounded_test::non_copyable_integer(5)})));
	return true;
}());

static_assert([]{
	auto x = container({5, 7});
	x.reserve(4_bi);
	containers::shrink_to_fit(x);
	BOUNDED_ASSERT(x.capacity() == 2_bi);
	BOUNDED_ASSERT(containers::equal(x, containers::vector({bounded_test::non_copyable_integer(5), bounded_test::non_copyable_integer(7)})));
	return true;
}());

} // namespace