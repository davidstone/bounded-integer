// Copyright David Stone 2024.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

export module bounded.test.tombstone_traits;

import bounded.arithmetic.unary_minus;
import bounded.comparison;
import bounded.integer;
import bounded.literal;
import bounded.tombstone_traits;

namespace {
using namespace bounded::literal;

struct a {
	int m;
};

struct b {
	constexpr b():
		m(1)
	{
	}
	a m;
private:
	constexpr explicit b(bounded::tombstone_tag, auto const make):
		m(make())
	{
	}
	friend bounded::tombstone_traits_composer<&b::m>;
};

} // namespace

template<>
struct bounded::tombstone_traits<a> {
	static constexpr auto spare_representations = 1_bi;
	static constexpr auto make(bounded::constant_t<0>) noexcept -> a {
		return a(0);
	}
	static constexpr auto index(a const & value) noexcept -> bounded::integer<-1, 0> {
		if (value.m == 0) {
			return 0_bi;
		}
		return -1_bi;
	}
};

template<>
struct bounded::tombstone_traits<b> : bounded::tombstone_traits_composer<&b::m> {
};

namespace {

using a_traits = bounded::tombstone_traits<a>;
static_assert(a_traits::spare_representations == 1_bi);
static_assert(a_traits::make(0_bi).m == 0);
static_assert(a_traits::index(a(0)) == 0_bi);
static_assert(a_traits::index(a(1)) == -1_bi);

using b_traits = bounded::tombstone_traits<b>;
static_assert(b_traits::spare_representations == 1_bi);
static_assert(b_traits::make(0_bi).m.m == 0);
static_assert(b_traits::index(b_traits::make(0_bi)) == 0_bi);
static_assert(b_traits::index(b()) == -1_bi);

} // namespace