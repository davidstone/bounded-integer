// Copyright David Stone 2024.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

export module tv.test.transform;

import tv.transform;
import tv.tuple;

import std_module;

struct {
	static constexpr auto operator()(float const x) {
		return static_cast<int>(x + 1);
	}
	static constexpr auto operator()(float const x, float const y) {
		return static_cast<int>(x + y);
	}
	static constexpr auto operator()(int const x) {
		return static_cast<float>(x + 1);
	}
	static constexpr auto operator()(int const x, int const y) {
		return static_cast<float>(x + y);
	}
} constexpr increment_and_swap_types{};

template<typename T>
constexpr auto equal(T const & lhs, std::same_as<T> auto const & rhs) {
	return lhs == rhs;
}

constexpr auto input_tuple = tv::tuple(0, 1.0F, 2, 3.0F, 4);

static_assert(equal(
	tv::transform(increment_and_swap_types, input_tuple),
	tv::tuple(1.0F, 2, 3.0F, 4, 5.0F)
));

static_assert(equal(
	tv::transform(increment_and_swap_types, input_tuple, input_tuple),
	tv::tuple(0.0F, 2, 4.0F, 6, 8.0F)
));