// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <containers/algorithms/concatenate.hpp>
#include <containers/vector.hpp>

#include <bounded/copy.hpp>

#include "../../test_assert.hpp"

namespace {

constexpr bool test() {
	using ::containers::begin;
	using ::containers::end;

	auto const a = containers::vector<int>{ 2, 6, 8 };
	auto const b = containers::vector<int>{ 3, 5 };
	auto const c = containers::vector<int>{ 1, 2, 3 };
	
	auto make_reusable_container = [&]{
		auto result = containers::vector<int>{};
		result.reserve(static_cast<containers::vector<int>::size_type>(size(a) + size(b) + size(c)));
		return result;
	};
	auto d = make_reusable_container();
	
	using Expected = containers::vector<int>;
	auto const expected_result = Expected{ 2, 6, 8, 3, 5, 1, 2, 3 };
	
	BOUNDED_TEST(::containers::concatenate<Expected>() == Expected{});
	BOUNDED_TEST(::containers::concatenate<Expected>(expected_result) == expected_result);
	BOUNDED_TEST(::containers::concatenate<Expected>(expected_result, Expected{}) == expected_result);
	BOUNDED_TEST(::containers::concatenate<Expected>(Expected{}, expected_result) == expected_result);
	
	BOUNDED_TEST(::containers::concatenate<Expected>(a, b, c) == expected_result);

	BOUNDED_TEST(::containers::concatenate<Expected>(bounded::copy(a), b, c) == expected_result);
	BOUNDED_TEST(::containers::concatenate<Expected>(a, bounded::copy(b), c) == expected_result);
	BOUNDED_TEST(::containers::concatenate<Expected>(a, b, bounded::copy(c)) == expected_result);

	BOUNDED_TEST(::containers::concatenate<Expected>(bounded::copy(a), bounded::copy(b), c) == expected_result);
	BOUNDED_TEST(::containers::concatenate<Expected>(bounded::copy(a), b, bounded::copy(c)) == expected_result);
	BOUNDED_TEST(::containers::concatenate<Expected>(a, bounded::copy(b), bounded::copy(c)) == expected_result);

	BOUNDED_TEST(::containers::concatenate<Expected>(bounded::copy(a), bounded::copy(b), bounded::copy(c)) == expected_result);

	BOUNDED_TEST(::containers::concatenate<Expected>(a, b, c, d) == expected_result);

	BOUNDED_TEST(::containers::concatenate<Expected>(bounded::copy(d), a, b, c) == expected_result);
	BOUNDED_TEST(::containers::concatenate<Expected>(a, bounded::copy(d), b, c) == expected_result);
	BOUNDED_TEST(::containers::concatenate<Expected>(a, b, bounded::copy(d), c) == expected_result);
	BOUNDED_TEST(::containers::concatenate<Expected>(a, b, c, bounded::copy(d)) == expected_result);

	BOUNDED_TEST(::containers::concatenate<Expected>(make_reusable_container(), a, b, c) == expected_result);
	BOUNDED_TEST(::containers::concatenate<Expected>(a, make_reusable_container(), b, c) == expected_result);
	BOUNDED_TEST(::containers::concatenate<Expected>(a, b, make_reusable_container(), c) == expected_result);
	BOUNDED_TEST(::containers::concatenate<Expected>(a, b, c, make_reusable_container()) == expected_result);
	
	BOUNDED_TEST(::containers::concatenate<Expected>(make_reusable_container(), a, make_reusable_container(), b, make_reusable_container(), c, make_reusable_container()) == expected_result);
	
	BOUNDED_TEST(::containers::concatenate<containers::vector<char>>(std::string_view("a"), std::string_view("b")) == containers::vector<char>({'a', 'b'}));
	return true;
}

static_assert(test());

} // namespace
