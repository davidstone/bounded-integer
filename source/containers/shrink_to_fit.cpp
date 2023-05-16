// Copyright David Stone 2020.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

module;

#include <bounded/assert.hpp>

export module containers.shrink_to_fit;

import containers.algorithms.compare;
import containers.algorithms.uninitialized;
import containers.begin_end;
import containers.is_range;
import containers.push_back;
import containers.size;
import containers.vector;

import bounded;
import bounded.test_int;
import numeric_traits;
import std_module;

using namespace bounded::literal;

namespace containers {

export template<range Container>
constexpr auto shrink_to_fit(Container & c) {
	auto const s = containers::size(c);
	if (s == c.capacity()) {
		return;
	}
	constexpr auto min_capacity = numeric_traits::min_value<decltype(c.capacity())>;
	if constexpr (min_capacity > 0_bi) {
		if (c.capacity() == min_capacity) {
			return;
		}
	}
	auto temp = Container();
	temp.reserve(s);
	containers::uninitialized_relocate_no_overlap(c, containers::begin(temp));
	temp.set_size(s);
	c.set_size(0_bi);
	c = std::move(temp);
}

} // namespace containers

namespace {

struct container {
	constexpr container() {
		m_data.reserve(1_bi);
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
	containers::vector<bounded_test::integer> m_data;
};

static_assert(container().capacity() == 1_bi);

static_assert([]{
	auto x = container();
	containers::shrink_to_fit(x);
	BOUNDED_ASSERT(x.capacity() == 1_bi);
	BOUNDED_ASSERT(containers::equal(x, containers::vector<bounded_test::integer>()));
	return true;
}());

static_assert([]{
	auto x = container();
	containers::push_back(x, 5);
	containers::shrink_to_fit(x);
	BOUNDED_ASSERT(x.capacity() == 1_bi);
	BOUNDED_ASSERT(containers::equal(x, containers::vector({bounded_test::integer(5)})));
	return true;
}());

static_assert([]{
	auto x = container();
	x.reserve(4_bi);
	containers::shrink_to_fit(x);
	BOUNDED_ASSERT(x.capacity() == 1_bi);
	BOUNDED_ASSERT(containers::equal(x, containers::vector<bounded_test::integer>()));
	return true;
}());

static_assert([]{
	auto x = container();
	x.reserve(4_bi);
	containers::push_back(x, 5);
	containers::shrink_to_fit(x);
	BOUNDED_ASSERT(x.capacity() == 1_bi);
	BOUNDED_ASSERT(containers::equal(x, containers::vector({bounded_test::integer(5)})));
	return true;
}());

static_assert([]{
	auto x = container();
	x.reserve(4_bi);
	containers::push_back(x, 5);
	containers::push_back(x, 7);
	containers::shrink_to_fit(x);
	BOUNDED_ASSERT(x.capacity() == 2_bi);
	BOUNDED_ASSERT(containers::equal(x, containers::vector({bounded_test::integer(5), bounded_test::integer(7)})));
	return true;
}());

} // namespace