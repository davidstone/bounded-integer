// Copyright David Stone 2019.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

module;

#include <operators/forward.hpp>

export module containers.algorithms.transform;

import containers.algorithms.compare;
import containers.algorithms.transform_traits;
import containers.array;
import containers.adapt;
import containers.default_begin_end_size;
import containers.range;

import std_module;

namespace containers {

template<typename UnaryFunction>
struct range_transform_traits : transform_traits<UnaryFunction>, default_begin_end_size {
	using transform_traits<UnaryFunction>::transform_traits;
};

template<typename UnaryFunction>
range_transform_traits(UnaryFunction) -> range_transform_traits<UnaryFunction>;

template<typename UnaryFunction>
struct range_transform_traits_dereference : transform_traits_dereference<UnaryFunction>, default_begin_end_size {
	using transform_traits_dereference<UnaryFunction>::transform_traits_dereference;
};

template<typename UnaryFunction>
range_transform_traits_dereference(UnaryFunction) -> range_transform_traits_dereference<UnaryFunction>;

export constexpr auto transform(range auto && source, auto dereference) -> range auto {
	return adapt(OPERATORS_FORWARD(source), range_transform_traits(std::move(dereference)));
}

export constexpr auto transform_dereference(range auto && source, auto dereference) -> range auto {
	return adapt(OPERATORS_FORWARD(source), range_transform_traits_dereference(std::move(dereference)));
}

} // namespace containers

struct S {
	constexpr auto f() const { return n; }

	friend auto operator==(S, S) -> bool = default;

	int n;
};

constexpr auto array = containers::array{S{0}, S{1}, S{2}, S{3}, S{4}};

static_assert(containers::equal(
	containers::transform(array, [](S const s) { return s; }),
	array
));

constexpr auto integer_array = containers::array{0, 1, 2, 3, 4};
static_assert(containers::equal(
	containers::transform(array, &S::f),
	integer_array
));

static_assert(containers::equal(
	containers::transform(array, [](S const s) { return s.f(); }),
	integer_array
));
