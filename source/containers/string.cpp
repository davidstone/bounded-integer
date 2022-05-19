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

TEST_CASE("string", "[string]") {
	containers_test::test_sequence_container<containers::string>();
	containers_test::test_reserve_and_capacity<containers::string>();
	containers_test::test_set_size<containers::string>();
}

} // namespace