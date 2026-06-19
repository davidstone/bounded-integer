// Copyright David Stone 2023.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

export module containers.test.array;

import containers.test.test_sequence_container;

import containers.array;
import containers.contiguous_range;
import containers.is_container;
import containers.range_value_t;
import containers.span;

import bounded;
import std_module;

namespace {
	
using namespace bounded::literal;
	
constexpr auto size = 5_bi;
constexpr containers::array<int, size> a = {};
static_assert(a.size() == size, "Incorrect size.");
static_assert(a[0_bi] == 0, "Incorrect value.");

static_assert(std::is_standard_layout_v<containers::array<int, 0_bi>>);
static_assert(std::is_trivially_default_constructible_v<containers::array<int, 0_bi>>);
static_assert(std::is_trivially_copyable_v<containers::array<int, 0_bi>>);
static_assert(std::is_trivially_copy_assignable_v<containers::array<int, 0_bi>>);
static_assert(std::is_trivially_destructible_v<containers::array<int, 0_bi>>);
static_assert(std::is_empty_v<containers::array<int, 0_bi>>);

template<typename T>
concept qualifiers_are_contiguous_ranges = 
	containers::contiguous_range<T const &> and
	containers::contiguous_range<T &>;

static_assert(qualifiers_are_contiguous_ranges<containers::array<int, 0_bi>>);
static_assert(qualifiers_are_contiguous_ranges<containers::array<int, 5_bi>>);

static_assert(containers::is_container<containers::array<int, 0_bi>>);
static_assert(containers::is_container<containers::array<int, 1_bi>>);

static_assert(bounded::convertible_to<containers::array<int, 0_bi> const &, containers::span<int const>>);
static_assert(containers::span<int const>(containers::array<int, 0_bi>()).size() == 0);
static_assert(!bounded::convertible_to<containers::array<int, 0_bi> const &, containers::span<int>>);
static_assert(bounded::convertible_to<containers::array<int, 0_bi> &, containers::span<int const>>);
static_assert(bounded::convertible_to<containers::array<int, 0_bi> &, containers::span<int>>);

static_assert(bounded::convertible_to<containers::array<int, 0_bi> const &, std::span<int const, 0>>);
static_assert(!bounded::convertible_to<containers::array<int, 0_bi> const &, std::span<int, 0>>);
static_assert(bounded::convertible_to<containers::array<int, 0_bi> &, std::span<int const, 0>>);
static_assert(bounded::convertible_to<containers::array<int, 0_bi> &, std::span<int, 0>>);

static_assert(bounded::convertible_to<containers::array<int, 3_bi> const &, containers::span<int const>>);
static_assert(!bounded::convertible_to<containers::array<int, 3_bi> const &, containers::span<int>>);
static_assert(bounded::convertible_to<containers::array<int, 3_bi> &, containers::span<int const>>);
static_assert(bounded::convertible_to<containers::array<int, 3_bi> &, containers::span<int>>);

static_assert(bounded::convertible_to<containers::array<int, 3_bi> const &, std::span<int const, 3>>);
static_assert(!bounded::convertible_to<containers::array<int, 3_bi> const &, std::span<int, 3>>);
static_assert(bounded::convertible_to<containers::array<int, 3_bi> &, std::span<int const, 3>>);
static_assert(bounded::convertible_to<containers::array<int, 3_bi> &, std::span<int, 3>>);

using namespace containers_test;

static_assert(test_sequence_container_default_constructed_empty<containers::array<int, 0_bi>>());
static_assert(test_sequence_container_implicit_from_two_empty_braces<containers::array<int, 0_bi>>());
static_assert(test_special_members<containers::array<int, 0_bi>>([] { return containers::array<int, 0_bi>({}); }));
static_assert(test_special_members<containers::array<int, 5_bi>>([] { return containers::array({0, 1, 2, 3, 4}); }));

} // namespace
