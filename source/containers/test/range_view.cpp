// Copyright David Stone 2024.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

export module containers.test.range_view;

import containers.algorithms.compare;
import containers.array;
import containers.contiguous_iterator;
import containers.has_member_size;
import containers.range;
import containers.range_view;

import bounded;
import std_module;

using namespace bounded::literal;

static_assert(containers::range_view<int *>(nullptr, nullptr).begin() == nullptr);
static_assert(containers::range_view(static_cast<int *>(nullptr), 0).data() == nullptr);
static_assert(containers::range_view(static_cast<int *>(nullptr), static_cast<int *>(nullptr)).data() == nullptr);
static_assert(containers::range_view<int *>(nullptr, nullptr).end() == nullptr);
static_assert(containers::range<containers::range_view<int *>>);

constexpr auto a = containers::array({0, 1, 2, 3, 4});
constexpr auto ctad_a = containers::range_view(a);
static_assert(sizeof(ctad_a) == sizeof(int const *));
static_assert(std::same_as<decltype(ctad_a.size()), bounded::constant_t<5>>);
static_assert(containers::equal(a, ctad_a));

constexpr int b[] = {0, 1, 2, 3, 4};
constexpr auto ctad_b = containers::range_view(b);
static_assert(sizeof(ctad_b) == sizeof(int const *));
static_assert(std::same_as<decltype(ctad_b.size()), bounded::constant_t<5>>);
static_assert(containers::equal(b, ctad_b));

static_assert(bounded::convertible_to<
	std::string_view,
	containers::range_view<int *, int *, std::size_t>
>);
static_assert(bounded::convertible_to<
	std::span<int>,
	containers::range_view<int *, int *, std::size_t>
>);
static_assert(bounded::convertible_to<
	std::span<int const>,
	containers::range_view<int *, int *, std::size_t>
>);
