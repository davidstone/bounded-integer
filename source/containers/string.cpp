// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <containers/string.hpp>

#include <cassert>

using namespace bounded::literal;

int main() {
	auto const from_string_literal = containers::string("David");
	assert(from_string_literal == from_string_literal);
	assert(from_string_literal == containers::string({'D', 'a', 'v', 'i', 'd'}));

	constexpr decltype(auto) array = "Stone"; // char const (&)[6]
	constexpr auto string_view = std::string_view(array);
	constexpr char const * c_string = array;

	auto const from_string_view = containers::string(string_view);
	assert(from_string_view == containers::string({'S', 't', 'o', 'n', 'e'}));
	assert(from_string_view == string_view);
	assert(string_view == from_string_view);

	auto const from_array = containers::string(array);
	assert(from_array == containers::string({'S', 't', 'o', 'n', 'e'}));
	assert(from_array == array);
	assert(array == from_array);

	auto const from_c_string = containers::string(c_string);
	assert(from_c_string == containers::string({'S', 't', 'o', 'n', 'e'}));
	assert(from_c_string == c_string);
	assert(c_string == from_c_string);

	// Verify this works in a range-based for loop	
	for (auto const & c [[maybe_unused]] : from_string_literal) {
	}
	
	assert(from_string_literal + from_string_view == "DavidStone");
	assert(from_string_literal + string_view == "DavidStone");
	assert(from_string_literal + array == "DavidStone");
	assert(from_string_literal + c_string == "DavidStone");

	assert(string_view + from_string_literal == "StoneDavid");
	assert(array + from_string_literal == "StoneDavid");
	assert(c_string + from_string_literal == "StoneDavid");
}
