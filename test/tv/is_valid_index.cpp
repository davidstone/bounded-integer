// Copyright David Stone 2023.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

export module tv.test.is_valid_index;

import tv.is_valid_index;

import bounded;

namespace {

template<typename Index, typename... Ts>
concept check_type_index =
	tv::matches_exactly_one_type<Index, Ts...> and
	tv::unique_type_identifier<bounded::type_t<Index>, Ts...>;

template<typename Index, typename... Ts>
concept check_integer_index =
	tv::variant_integer_index<Index, sizeof...(Ts)> and
	tv::unique_type_identifier<Index, Ts...>;

static_assert(!check_type_index<int>);
static_assert(check_type_index<int, int>);
static_assert(!check_type_index<int, short>);
static_assert(check_type_index<int, int, short>);
static_assert(!check_type_index<int, int, int>);

static_assert(!check_integer_index<bounded::constant_t<0>>);
static_assert(check_integer_index<bounded::constant_t<0>, int>);
static_assert(!check_integer_index<bounded::constant_t<1>, int>);
static_assert(check_integer_index<bounded::constant_t<0>, int, short>);
static_assert(check_integer_index<bounded::constant_t<1>, int, int>);

} // namespace