// Copyright David Stone 2020.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <bounded/detail/integer_tombstone_traits.hpp>
#include <bounded/detail/literal.hpp>

namespace {

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
	pair(0_bi, 5_bi),
	pair(1_bi, 6_bi),
	pair(2_bi, 7_bi),
	pair(3_bi, 8_bi),
	pair(4_bi, 9_bi),
	pair(5_bi, 10_bi),
	pair(250_bi, 255_bi)
));

static_assert(bounded::tombstone_traits<bounded::integer<2, 5>>::spare_representations == 252_bi);

static_assert(reversible<bounded::integer<2, 5>>(
	pair(0_bi, 0_bi),
	pair(1_bi, 1_bi),
	pair(2_bi, 6_bi),
	pair(3_bi, 7_bi),
	pair(4_bi, 8_bi),
	pair(5_bi, 9_bi),
	pair(251_bi, 255_bi)
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