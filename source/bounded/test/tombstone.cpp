// Copyright David Stone 2024.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

export module bounded.test.tombstone;

import bounded.arithmetic.unary_minus;
import bounded.comparison;
import bounded.integer;
import bounded.literal;
import bounded.tombstone;

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
	friend bounded::tombstone_member<&b::m>;
};

} // namespace

template<>
struct bounded::tombstone<a> {
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
struct bounded::tombstone<b> : bounded::tombstone_member<&b::m> {
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


using namespace bounded::literal;

template<typename Index, typename Value>
struct pair {
	constexpr pair(Index index_, Value value_):
		index(index_),
		value(value_)
	{
	}
	Index index;
	Value value;
};

template<typename T>
constexpr auto reversible(auto const input) {
	constexpr auto const value = bounded::tombstone_traits<T>::make(input.index);
	static_assert(bounded::integer(value.m_value) == input.value);

	constexpr auto index = bounded::tombstone_traits<T>::index(value);
	static_assert(index == input.index);
}

template<typename T>
constexpr auto reversible(auto const ... indexes) {
	(..., reversible<T>(indexes));
	return true;
}

static_assert(bounded::tombstone_traits<bounded::integer<0, 4>>::spare_representations == 251_bi);

static_assert(reversible<bounded::integer<0, 4>>(
	pair(0_bi, -128_bi),
	pair(1_bi, -127_bi),
	pair(128_bi, 5_bi),
	pair(250_bi, 127_bi)
));

static_assert(bounded::tombstone_traits<bounded::integer<2, 5>>::spare_representations == 252_bi);

static_assert(reversible<bounded::integer<2, 5>>(
	pair(0_bi, -128_bi),
	pair(1_bi, -127_bi),
	pair(129_bi, 1_bi),
	pair(130_bi, 6_bi),
	pair(251_bi, 127_bi)
));

static_assert(bounded::tombstone_traits<bounded::integer<254, 255>>::spare_representations == 254_bi);

static_assert(reversible<bounded::integer<254, 255>>(
	pair(0_bi, 0_bi),
	pair(1_bi, 1_bi),
	pair(2_bi, 2_bi),
	pair(3_bi, 3_bi),
	pair(251_bi, 251_bi),
	pair(252_bi, 252_bi),
	pair(253_bi, 253_bi)
));

static_assert(bounded::tombstone_traits<bounded::integer<-128, -5>>::spare_representations == 132_bi);

static_assert(reversible<bounded::integer<-128, -5>>(
	pair(0_bi, -4_bi),
	pair(1_bi, -3_bi),
	pair(131_bi, 127_bi)
));

static_assert(bounded::tombstone_traits<bounded::integer<-50, 127>>::spare_representations == 78_bi);

static_assert(reversible<bounded::integer<-50, 127>>(
	pair(0_bi, -128_bi),
	pair(1_bi, -127_bi),
	pair(77_bi, -51_bi)
));

static_assert(bounded::tombstone_traits<bounded::integer<-50, 49>>::spare_representations == 156_bi);

static_assert(reversible<bounded::integer<-50, 49>>(
	pair(0_bi, -128_bi),
	pair(1_bi, -127_bi),
	pair(77_bi, -51_bi),
	pair(78_bi, 50_bi),
	pair(79_bi, 51_bi),
	pair(155_bi, 127_bi)
));

static_assert(bounded::tombstone_traits<bounded::integer<0, 255>>::spare_representations == 0_bi);
static_assert(bounded::tombstone_traits<bounded::integer<-128, 127>>::spare_representations == 0_bi);
static_assert(bounded::tombstone_traits<bounded::integer<53, 53>>::spare_representations == 0_bi);

} // namespace