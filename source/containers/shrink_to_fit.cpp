// Copyright David Stone 2020.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <containers/shrink_to_fit.hpp>

#include <containers/vector.hpp>

#include "../test_assert.hpp"
#include "../test_int.hpp"

namespace {
using namespace bounded::literal;

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
		return bounded::integer<1, 5>(m_data.capacity());
	}
	constexpr void append_from_capacity(auto const added) {
		m_data.append_from_capacity(added);
	}

private:
	containers::vector<bounded::test_int> m_data;
};

static_assert(container().capacity() == 1_bi);

static_assert([]{
	auto x = container();
	containers::shrink_to_fit(x);
	BOUNDED_TEST(x.capacity() == 1_bi);
	BOUNDED_TEST(containers::equal(x, containers::vector<bounded::test_int>()));
	return true;
}());

static_assert([]{
	auto x = container();
	containers::push_back(x, 5);
	containers::shrink_to_fit(x);
	BOUNDED_TEST(x.capacity() == 1_bi);
	BOUNDED_TEST(containers::equal(x, containers::vector({bounded::test_int(5)})));
	return true;
}());

static_assert([]{
	auto x = container();
	x.reserve(4_bi);
	containers::shrink_to_fit(x);
	BOUNDED_TEST(x.capacity() == 1_bi);
	BOUNDED_TEST(containers::equal(x, containers::vector<bounded::test_int>()));
	return true;
}());

static_assert([]{
	auto x = container();
	x.reserve(4_bi);
	containers::push_back(x, 5);
	containers::shrink_to_fit(x);
	BOUNDED_TEST(x.capacity() == 1_bi);
	BOUNDED_TEST(containers::equal(x, containers::vector({bounded::test_int(5)})));
	return true;
}());

static_assert([]{
	auto x = container();
	x.reserve(4_bi);
	containers::push_back(x, 5);
	containers::push_back(x, 7);
	containers::shrink_to_fit(x);
	BOUNDED_TEST(x.capacity() == 2_bi);
	BOUNDED_TEST(containers::equal(x, containers::vector({bounded::test_int(5), bounded::test_int(7)})));
	return true;
}());

} // namespace