// Copyright David Stone 2020.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

module;

#undef NDEBUG

#include <bounded/assert.hpp>

export module containers.test.test_reserve_and_capacity;

import containers.data;
import containers.range_size_t;
import containers.range_value_t;
import containers.size;

import bounded;

namespace containers_test {

using namespace bounded::literal;

template<typename Container>
constexpr auto test_empty_capacity_not_negative() -> void {
	auto v = Container();
	BOUNDED_ASSERT(v.capacity() >= 0_bi);
}

template<typename Container>
constexpr auto test_non_empty_capacity_positive() -> void {
	auto v = Container({containers::range_value_t<Container>()});
	BOUNDED_ASSERT(v.capacity() > 0_bi);
}

template<typename Container>
constexpr auto test_reserve_one_more_from_empty() -> void {
	auto v = Container();
	auto const initial_capacity = v.capacity();
	v.reserve(::bounded::assume_in_range<containers::range_size_t<Container>>(v.capacity() + 1_bi));
	auto const capacity1 = v.capacity();
	BOUNDED_ASSERT(capacity1 >= 1_bi);
	BOUNDED_ASSERT(capacity1 >= initial_capacity + 1_bi);
	BOUNDED_ASSERT(containers::size(v) == 0_bi);
}

template<typename Container>
constexpr auto test_reserve_one_more_from_non_empty() -> void {
	auto v = Container({containers::range_value_t<Container>()});
	auto const initial_capacity = v.capacity();
	v.reserve(::bounded::assume_in_range<containers::range_size_t<Container>>(v.capacity() + 1_bi));
	BOUNDED_ASSERT(v.capacity() >= initial_capacity + 1_bi);
	BOUNDED_ASSERT(v == Container({containers::range_value_t<Container>()}));
}

template<typename Container>
constexpr auto test_reserve_smaller_size_no_op() -> void {
	auto v = Container();
	v.reserve(::bounded::assume_in_range<containers::range_size_t<Container>>(v.capacity() + 1_bi));
	auto const initial_capacity = v.capacity();
	auto const ptr = containers::data(v);
	v.reserve(0_bi);
	BOUNDED_ASSERT(v.capacity() == initial_capacity);
	BOUNDED_ASSERT(containers::size(v) == 0_bi);
	BOUNDED_ASSERT(containers::data(v) == ptr);
}

template<typename Container>
constexpr auto test_reserve_same_size_no_op() -> void {
	auto v = Container();
	v.reserve(::bounded::assume_in_range<containers::range_size_t<Container>>(v.capacity() + 1_bi));
	auto const initial_capacity = v.capacity();
	auto const ptr = containers::data(v);
	v.reserve(initial_capacity);
	BOUNDED_ASSERT(v.capacity() == initial_capacity);
	BOUNDED_ASSERT(containers::size(v) == 0_bi);
	BOUNDED_ASSERT(containers::data(v) == ptr);
}

export template<typename Container>
constexpr auto test_reserve_and_capacity() -> bool {
	test_empty_capacity_not_negative<Container>();
	test_non_empty_capacity_positive<Container>();
	test_reserve_one_more_from_empty<Container>();
	test_reserve_one_more_from_non_empty<Container>();
	test_reserve_smaller_size_no_op<Container>();
	test_reserve_same_size_no_op<Container>();
	return true;
}

} // namespace containers_test