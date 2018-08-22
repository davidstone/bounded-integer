// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <containers/algorithms/concatenate.hpp>
#include <containers/string.hpp>
#include <containers/vector/vector.hpp>

#include <iostream>
#include <string_view>

template<typename LHS, typename RHS>
void assert_eq(LHS const & lhs, RHS const & rhs, std::size_t const line) {
	if (lhs != rhs) {
		auto print = [](auto const & container) {
			std::cerr << "{ ";
			for (auto const & value : container) {
				std::cerr << value << ", ";
			}
			std::cerr << "}\n";
		};
		std::cerr << "Failure at line " << line << '\n';
		print(lhs);
		print(rhs);
		std::cerr << '\n';
	}
}

#ifdef ASSERT_EQ
#undef ASSERT_EQ
#endif

#define ASSERT_EQ(lhs, rhs) assert_eq(lhs, rhs, __LINE__)

auto main() -> int {
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
	
	ASSERT_EQ(::containers::concatenate<Expected>(), Expected{});
	ASSERT_EQ(::containers::concatenate<Expected>(expected_result), expected_result);
	ASSERT_EQ(::containers::concatenate<Expected>(expected_result, Expected{}), expected_result);
	ASSERT_EQ(::containers::concatenate<Expected>(Expected{}, expected_result), expected_result);
	
	ASSERT_EQ(::containers::concatenate<Expected>(a, b, c), expected_result);

	ASSERT_EQ(::containers::concatenate<Expected>(::containers::copy(a), b, c), expected_result);
	ASSERT_EQ(::containers::concatenate<Expected>(a, ::containers::copy(b), c), expected_result);
	ASSERT_EQ(::containers::concatenate<Expected>(a, b, ::containers::copy(c)), expected_result);

	ASSERT_EQ(::containers::concatenate<Expected>(::containers::copy(a), ::containers::copy(b), c), expected_result);
	ASSERT_EQ(::containers::concatenate<Expected>(::containers::copy(a), b, ::containers::copy(c)), expected_result);
	ASSERT_EQ(::containers::concatenate<Expected>(a, ::containers::copy(b), ::containers::copy(c)), expected_result);

	ASSERT_EQ(::containers::concatenate<Expected>(::containers::copy(a), ::containers::copy(b), ::containers::copy(c)), expected_result);

	ASSERT_EQ(::containers::concatenate<Expected>(a, b, c, d), expected_result);

	ASSERT_EQ(::containers::concatenate<Expected>(::containers::copy(d), a, b, c), expected_result);
	ASSERT_EQ(::containers::concatenate<Expected>(a, ::containers::copy(d), b, c), expected_result);
	ASSERT_EQ(::containers::concatenate<Expected>(a, b, ::containers::copy(d), c), expected_result);
	ASSERT_EQ(::containers::concatenate<Expected>(a, b, c, ::containers::copy(d)), expected_result);

	ASSERT_EQ(::containers::concatenate<Expected>(make_reusable_container(), a, b, c), expected_result);
	ASSERT_EQ(::containers::concatenate<Expected>(a, make_reusable_container(), b, c), expected_result);
	ASSERT_EQ(::containers::concatenate<Expected>(a, b, make_reusable_container(), c), expected_result);
	ASSERT_EQ(::containers::concatenate<Expected>(a, b, c, make_reusable_container()), expected_result);
	
	ASSERT_EQ(::containers::concatenate<Expected>(make_reusable_container(), a, make_reusable_container(), b, make_reusable_container(), c, make_reusable_container()), expected_result);
}
