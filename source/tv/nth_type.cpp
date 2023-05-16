// Copyright David Stone 2020.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

export module tv.nth_type;

import std_module;

namespace tv {

template<std::size_t index, typename T, typename... Ts>
struct nth_type_c {
	using type = typename nth_type_c<index - 1, Ts...>::type;
};
template<typename T, typename... Ts>
struct nth_type_c<0, T, Ts...> {
	using type = T;
};

export template<std::size_t index, typename... Ts>
using nth_type = typename nth_type_c<index, Ts...>::type;

} // namespace tv

static_assert(std::same_as<tv::nth_type<0, int>, int>);
static_assert(std::same_as<tv::nth_type<0, int, long>, int>);
static_assert(std::same_as<tv::nth_type<1, int, long>, long>);
