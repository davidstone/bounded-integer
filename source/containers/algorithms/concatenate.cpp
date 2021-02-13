// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <containers/algorithms/concatenate.hpp>
#include <containers/vector.hpp>

#include <bounded/copy.hpp>

#include "../../test_assert.hpp"
#include "../../test_int.hpp"

namespace {

constexpr bool test() {
	using ::containers::begin;
	using ::containers::end;
	using Container = containers::vector<bounded::test_int>;

	auto const a = Container({2, 6, 8});
	auto const b = Container({3, 5});
	auto const c = Container({1, 2, 3});
	
	auto make_reusable_container = [&]{
		auto result = Container();
		result.reserve(static_cast<Container::size_type>(containers::size(a) + containers::size(b) + containers::size(c)));
		return result;
	};
	auto const d = make_reusable_container();
	
	auto const expected_result = Container({2, 6, 8, 3, 5, 1, 2, 3});
	
	BOUNDED_TEST(::containers::concatenate<Container>() == Container());
	BOUNDED_TEST(::containers::concatenate<Container>(expected_result) == expected_result);
	BOUNDED_TEST(::containers::concatenate<Container>(expected_result, Container()) == expected_result);
	BOUNDED_TEST(::containers::concatenate<Container>(Container(), expected_result) == expected_result);
	
	BOUNDED_TEST(::containers::concatenate<Container>(a, b, c) == expected_result);

	BOUNDED_TEST(::containers::concatenate<Container>(bounded::copy(a), b, c) == expected_result);
	BOUNDED_TEST(::containers::concatenate<Container>(a, bounded::copy(b), c) == expected_result);
	BOUNDED_TEST(::containers::concatenate<Container>(a, b, bounded::copy(c)) == expected_result);

	BOUNDED_TEST(::containers::concatenate<Container>(bounded::copy(a), bounded::copy(b), c) == expected_result);
	BOUNDED_TEST(::containers::concatenate<Container>(bounded::copy(a), b, bounded::copy(c)) == expected_result);
	BOUNDED_TEST(::containers::concatenate<Container>(a, bounded::copy(b), bounded::copy(c)) == expected_result);

	BOUNDED_TEST(::containers::concatenate<Container>(bounded::copy(a), bounded::copy(b), bounded::copy(c)) == expected_result);

	BOUNDED_TEST(::containers::concatenate<Container>(a, b, c, d) == expected_result);

	BOUNDED_TEST(::containers::concatenate<Container>(bounded::copy(d), a, b, c) == expected_result);
	BOUNDED_TEST(::containers::concatenate<Container>(a, bounded::copy(d), b, c) == expected_result);
	BOUNDED_TEST(::containers::concatenate<Container>(a, b, bounded::copy(d), c) == expected_result);
	BOUNDED_TEST(::containers::concatenate<Container>(a, b, c, bounded::copy(d)) == expected_result);

	BOUNDED_TEST(::containers::concatenate<Container>(make_reusable_container(), a, b, c) == expected_result);
	BOUNDED_TEST(::containers::concatenate<Container>(a, make_reusable_container(), b, c) == expected_result);
	BOUNDED_TEST(::containers::concatenate<Container>(a, b, make_reusable_container(), c) == expected_result);
	BOUNDED_TEST(::containers::concatenate<Container>(a, b, c, make_reusable_container()) == expected_result);
	
	BOUNDED_TEST(::containers::concatenate<Container>(make_reusable_container(), a, make_reusable_container(), b, make_reusable_container(), c, make_reusable_container()) == expected_result);
	
	BOUNDED_TEST(::containers::concatenate<containers::vector<char>>(std::string_view("a"), std::string_view("b")) == containers::vector<char>({'a', 'b'}));
	return true;
}

static_assert(test());

} // namespace
