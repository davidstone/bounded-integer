// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <containers/string.hpp>

#include <containers/repeat_n.hpp>

#include "../test_assert.hpp"

namespace {

using namespace bounded::literal;

constexpr bool test() {
	auto const from_string_literal = containers::string("David");
	BOUNDED_TEST(from_string_literal == from_string_literal);
	BOUNDED_TEST(from_string_literal == containers::string({'D', 'a', 'v', 'i', 'd'}));

	constexpr decltype(auto) array = "Stone"; // char const (&)[6]
	constexpr auto string_view = std::string_view(array);
	constexpr char const * c_string = array;

	auto const from_string_view = containers::string(string_view);
	BOUNDED_TEST(from_string_view == containers::string({'S', 't', 'o', 'n', 'e'}));
	BOUNDED_TEST(from_string_view == string_view);
	BOUNDED_TEST(string_view == from_string_view);

	auto const from_array = containers::string(array);
	BOUNDED_TEST(from_array == containers::string({'S', 't', 'o', 'n', 'e'}));
	BOUNDED_TEST(from_array == array);
	BOUNDED_TEST(array == from_array);

	auto const from_c_string = containers::string(c_string);
	BOUNDED_TEST(from_c_string == containers::string({'S', 't', 'o', 'n', 'e'}));
	BOUNDED_TEST(from_c_string == c_string);
	BOUNDED_TEST(c_string == from_c_string);

	// Verify this works in a range-based for loop	
	for (auto const & c [[maybe_unused]] : from_string_literal) {
	}

#if 0
	auto small_to_large = containers::string("a");
	small_to_large = containers::string(containers::repeat_n(40_bi, 'c'));
#endif
	
	return true;
}

static_assert(test());

} // namespace