// Copyright David Stone 2019.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <containers/algorithms/transform.hpp>
#include <containers/algorithms/compare.hpp>
#include <containers/array.hpp>

namespace {

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

} // namespace
