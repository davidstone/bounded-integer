// Copyright David Stone 2017.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <containers/array.hpp>
#include <containers/begin_end.hpp>
#include <containers/data.hpp>
#include <containers/range_value_t.hpp>
#include <containers/size.hpp>

#include <bounded/integer.hpp>

#include "test_sequence_container.hpp"

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
static_assert(containers_test::test_sequence_container_implicit_from_two_empty_braces<containers::array<int, 0>>());
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

constexpr auto array_n = containers::make_array_n(6_bi, 5);
static_assert(std::is_same_v<containers::range_value_t<decltype(array_n)>, int>, "Incorrect type from make_array_n.");
static_assert(containers::size(array_n) == 6_bi, "Incorrect size from make_array_n.");
static_assert(array_n[3_bi] == 5, "Incorrect values from make_array_n.");

struct non_copyable {
	non_copyable() = default;
	non_copyable(non_copyable const &) = delete;
	non_copyable(non_copyable &&) = default;
};

constexpr auto array_non_copyable = containers::make_array_n(1_bi, non_copyable());
static_assert(containers::size(array_non_copyable) == 1_bi);

constexpr auto array_empty = containers::make_array_n(0_bi, 2);
static_assert(array_empty.size() == 0_bi);

constexpr auto array_non_copyable_empty = containers::make_array_n(0_bi, non_copyable());
static_assert(array_non_copyable_empty.size() == 0_bi);

constexpr auto large_size = 10000_bi;
constexpr auto large_array_n = containers::make_array_n(large_size, 0);
static_assert(containers::size(large_array_n) == large_size);

} // namespace
