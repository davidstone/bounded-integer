// Copyright David Stone 2019.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

export module containers.algorithms.test.transform;

import containers.algorithms.compare;
import containers.algorithms.transform;
import containers.array;

import bounded;

using namespace bounded::literal;

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

static_assert(containers::equal(
	containers::transform_non_idempotent(array, [](S const s) { return s; }),
	array
));

constexpr auto integer_array = containers::array{0, 1, 2, 3, 4};

static_assert(containers::equal(
	containers::transform(array, &S::f),
	integer_array
));

static_assert(containers::equal(
	containers::transform_non_idempotent(array, &S::f),
	integer_array
));

static_assert(containers::equal(
	containers::transform(array, [](S const s) { return s.f(); }),
	integer_array
));

static_assert(containers::equal(
	containers::transform_non_idempotent(array, [](S const s) { return s.f(); }),
	integer_array
));

#if 0
// It looks impossible to fix this problem with the iterator model
auto _() -> void {
	for (auto _ : containers::transform(containers::array<int, 0_bi>(), [](int &&) { return 0; })) {
	}
	for (auto _ : containers::transform_non_idempotent(containers::array<int, 0_bi>(), [](int &&) { return 0; })) {
	}
}
#endif
