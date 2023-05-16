// Copyright David Stone 2023.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

module;

#include <bounded/assert.hpp>

export module containers.test.algorithms.concatenate;

import containers.algorithms.concatenate;
import containers.begin_end;
import containers.size;
import containers.vector;

import bounded;
import bounded.test_int;
import std_module;

static_assert([] {
	using ::containers::begin;
	using ::containers::end;
	using Container = containers::vector<bounded_test::integer>;

	auto const a = Container({2, 6, 8});
	auto const b = Container({3, 5});
	auto const c = Container({1, 2, 3});
	
	auto reserve_extra_capacity = [&](auto container) {
		container.reserve(::bounded::assume_in_range<containers::range_size_t<Container>>(containers::size(a) + containers::size(b) + containers::size(c)));
		return container;
	};
	
	auto const expected_result = Container({2, 6, 8, 3, 5, 1, 2, 3});
	
	BOUNDED_ASSERT(::containers::concatenate<Container>() == Container());
	BOUNDED_ASSERT(::containers::concatenate<Container>(expected_result) == expected_result);
	BOUNDED_ASSERT(::containers::concatenate<Container>(Container({1})) == Container({1}));
	BOUNDED_ASSERT(::containers::concatenate<Container>(expected_result, Container()) == expected_result);
	BOUNDED_ASSERT(::containers::concatenate<Container>(Container(), expected_result) == expected_result);
	
	BOUNDED_ASSERT(::containers::concatenate<Container>(a, b, c) == expected_result);

	BOUNDED_ASSERT(::containers::concatenate<Container>(bounded::copy(a), b, c) == expected_result);
	BOUNDED_ASSERT(::containers::concatenate<Container>(a, bounded::copy(b), c) == expected_result);
	BOUNDED_ASSERT(::containers::concatenate<Container>(a, b, bounded::copy(c)) == expected_result);

	BOUNDED_ASSERT(::containers::concatenate<Container>(bounded::copy(a), bounded::copy(b), c) == expected_result);
	BOUNDED_ASSERT(::containers::concatenate<Container>(bounded::copy(a), b, bounded::copy(c)) == expected_result);
	BOUNDED_ASSERT(::containers::concatenate<Container>(a, bounded::copy(b), bounded::copy(c)) == expected_result);

	BOUNDED_ASSERT(::containers::concatenate<Container>(bounded::copy(a), bounded::copy(b), bounded::copy(c)) == expected_result);

	auto const extra_capacity_a = reserve_extra_capacity(a);
	BOUNDED_ASSERT(::containers::concatenate<Container>(extra_capacity_a, b, c) == expected_result);

	BOUNDED_ASSERT(::containers::concatenate<Container>(reserve_extra_capacity(a), b, c) == expected_result);
	BOUNDED_ASSERT(::containers::concatenate<Container>(a, reserve_extra_capacity(b), c) == expected_result);
	BOUNDED_ASSERT(::containers::concatenate<Container>(a, b, reserve_extra_capacity(c)) == expected_result);

	BOUNDED_ASSERT(::containers::concatenate<Container>(reserve_extra_capacity(a), bounded::copy(b), c) == expected_result);
	
	BOUNDED_ASSERT(::containers::concatenate<Container>(Container(), a, Container(), b, Container(), c, Container()) == expected_result);
	BOUNDED_ASSERT(::containers::concatenate<Container>(reserve_extra_capacity(a), reserve_extra_capacity(b), reserve_extra_capacity(c)) == expected_result);
	
	BOUNDED_ASSERT(::containers::concatenate<containers::vector<char>>(std::string_view("a"), std::string_view("b")) == containers::vector<char>({'a', 'b'}));

	using non_copyable_container = containers::vector<bounded_test::non_copyable_integer>;
	BOUNDED_ASSERT(::containers::concatenate<non_copyable_container>(non_copyable_container({1})) == non_copyable_container({1}));
	return true;
}());
