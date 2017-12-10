// Copyright David Stone 2016.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <containers/string.hpp>

#include <cassert>

using namespace bounded::literal;

int main() {
	auto const from_string_literal = containers::string("David");
	decltype(auto) array = "Stone";	// char const (&)[6]
	auto const c_string = array;
	auto const from_string_array = containers::string(array);
	assert(from_string_array == array);

	auto const from_c_string = containers::string(c_string);
	assert(from_c_string == c_string);
	assert(c_string == from_c_string);
	assert(from_c_string == containers::string({'S', 't', 'o', 'n', 'e'}));

	auto const from_array_with_size = containers::string(array, 2_bi);
	assert(size(from_array_with_size) == 2_bi);
	assert(::containers::equal(from_array_with_size.begin(), from_array_with_size.end(), array, array + 2_bi));
	assert(from_array_with_size == containers::string({'S', 't'}));

	//auto const fails_to_compile = containers::string(array, 10_bi);

	auto const from_c_string_with_size = containers::string(c_string, 1_bi);
	assert(::containers::equal(from_c_string_with_size.begin(), from_c_string_with_size.end(), c_string, c_string + 1_bi));
	assert(from_c_string_with_size == containers::string({'S'}));
	assert(from_c_string_with_size == "S");
	assert("S" == from_c_string_with_size);

	assert(from_c_string_with_size < "T");
	assert("R" < from_c_string_with_size);
	assert(from_c_string_with_size < "SS");
	assert("RR" < from_c_string_with_size);
	assert(from_array_with_size != from_c_string);
	assert("R" <= from_c_string_with_size);
}
