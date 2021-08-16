// Copyright David Stone 2017.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <containers/array/array.hpp>
#include <containers/begin_end.hpp>
#include <containers/data.hpp>
#include <containers/size.hpp>

#include <bounded/integer.hpp>

#include "../test_sequence_container.hpp"

namespace {
using namespace bounded::literal;

constexpr auto size = 5;
constexpr containers::array<int, size> a = {};
static_assert(containers::size(a) == size, "Incorrect size.");
static_assert(a[0_bi] == 0, "Incorrect value.");
static_assert(containers::end(a) - containers::begin(a) == size, "Incorrect difference type.");

static_assert(std::is_standard_layout_v<containers::array<int, 0>>);
static_assert(std::is_trivial_v<containers::array<int, 0>>);
static_assert(std::is_empty_v<containers::array<int, 0>>);

template<typename T>
concept qualifiers_are_contiguous_ranges = 
	containers::contiguous_range<T const &> and
	containers::contiguous_range<T &>;

static_assert(qualifiers_are_contiguous_ranges<containers::array<int, 0>>);
static_assert(qualifiers_are_contiguous_ranges<containers::array<int, 5>>);

static_assert(containers_test::test_sequence_container_default_constructed_empty<containers::array<int, 0>>());
static_assert(containers_test::test_special_members(containers::array<int, 0>({})));
static_assert(containers_test::test_special_members(containers::array<int, 5>({0, 1, 2, 3, 4})));

static_assert(std::is_convertible_v<containers::array<int, 3> const &, std::span<int const>>);
static_assert(!std::is_convertible_v<containers::array<int, 3> const &, std::span<int>>);
static_assert(std::is_convertible_v<containers::array<int, 3> &, std::span<int const>>);
static_assert(std::is_convertible_v<containers::array<int, 3> &, std::span<int>>);

static_assert(std::is_convertible_v<containers::array<int, 3> const &, std::span<int const, 3>>);
static_assert(!std::is_convertible_v<containers::array<int, 3> const &, std::span<int, 3>>);
static_assert(std::is_convertible_v<containers::array<int, 3> &, std::span<int const, 3>>);
static_assert(std::is_convertible_v<containers::array<int, 3> &, std::span<int, 3>>);

}	// namespace
