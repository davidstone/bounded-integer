// Copyright David Stone 2024.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

export module tv.test.tuple_size;

import tv.tuple;
import tv.tuple_size;

import bounded;
import numeric_traits;
import std_module;

using namespace bounded::literal;

static_assert(tv::tuple_size<tv::tuple<>> == 0_bi);
static_assert(tv::tuple_size<tv::tuple<int>> == 1_bi);
static_assert(tv::tuple_size<tv::tuple<int, int>> == 2_bi);
static_assert(tv::tuple_size<tv::tuple<> const> == 0_bi);
static_assert(tv::tuple_size<tv::tuple<> volatile> == 0_bi);
static_assert(tv::tuple_size<tv::tuple<> const volatile> == 0_bi);
static_assert(tv::tuple_size<tv::tuple<> &> == 0_bi);
static_assert(tv::tuple_size<tv::tuple<> const &> == 0_bi);
static_assert(tv::tuple_size<tv::tuple<> &&> == 0_bi);
static_assert(std::same_as<
	decltype(tv::tuple_size<int>),
	numeric_traits::incomplete
>);
static_assert(std::same_as<
	decltype(tv::tuple_size<int const>),
	numeric_traits::incomplete
>);
