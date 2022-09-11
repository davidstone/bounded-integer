// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <containers/string.hpp>

#include <containers/c_array.hpp>

#include <catch2/catch_test_macros.hpp>

#include "test_reserve_and_capacity.hpp"
#include "test_sequence_container.hpp"
#include "test_set_size.hpp"

namespace {

struct to_sv {
	constexpr operator std::string_view() const {
		return "";
	}
};

struct to_cstr {
	constexpr operator char const *() const {
		return "";
	}
};

struct to_both {
	constexpr operator char const *() const {
		return "";
	}
	constexpr operator std::string_view() const {
		return "";
	}
};

static_assert(std::convertible_to<char const *, containers::string>);
static_assert(std::convertible_to<char *, containers::string>);
static_assert(std::convertible_to<containers::c_array<char, 5> const &, containers::string>);
static_assert(std::convertible_to<containers::c_array<char, 5> &, containers::string>);
static_assert(std::convertible_to<std::string_view, containers::string>);
static_assert(!std::convertible_to<to_sv, containers::string>);
static_assert(!std::convertible_to<to_cstr, containers::string>);
static_assert(!std::convertible_to<to_both, containers::string>);
static_assert(std::constructible_from<containers::string, to_sv>);
static_assert(std::constructible_from<containers::string, to_cstr>);
static_assert(std::constructible_from<containers::string, to_both>);

constexpr auto check_equal(std::string_view const input) {
	auto const output = containers::string(input);
	BOUNDED_TEST(output == input);
	BOUNDED_TEST(containers::size(output) == containers::size(input));
	return true;
}

static_assert(check_equal(""));
static_assert(check_equal("a"));
static_assert(check_equal("01234567890123456789012"));

TEST_CASE("string", "[string]") {
	containers_test::test_sequence_container<containers::string>();
	containers_test::test_reserve_and_capacity<containers::string>();
	containers_test::test_set_size<containers::string>();
	check_equal("012345678901234567890123");
	check_equal("0123456789012345678901234");
}

} // namespace