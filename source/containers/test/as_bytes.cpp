// Copyright David Stone 2026.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

module;

#include <doctest/doctest.h>

export module containers.test.as_bytes;

import containers.algorithms.compare;
import containers.array;
import containers.as_bytes;
import containers.front;

import std_module;

TEST_CASE("as_bytes") {
	CHECK(containers::equal(
		containers::as_bytes(containers::array{std::uint32_t(0)}),
		containers::array{std::byte(0x00), std::byte(0x00), std::byte(0x00), std::byte(0x00)}
	));
}

TEST_CASE("as_writeable_bytes") {
	auto source = containers::array{0};
	auto const bytes = containers::as_writeable_bytes(source);
	containers::front(bytes) = std::byte(0x01);
	CHECK(containers::front(source) != 0);
}
