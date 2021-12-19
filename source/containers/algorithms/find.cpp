// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <containers/algorithms/find.hpp>

#include <containers/vector.hpp>

namespace {

using namespace bounded::literal;

constexpr auto test(auto const index, containers::vector<int> const & v) -> bool {
	auto it = containers::find(v, 5);
	BOUNDED_ASSERT(index == containers::size(v) or *it == 5);
	return it - containers::begin(v) == index;
}
template<std::size_t size>
constexpr auto test(auto const index, containers::c_array<int, size> && a) -> bool {
	return test(index, containers::vector<int>(std::move(a)));
}

static_assert(test(0_bi, {}));
static_assert(test(1_bi, {0}));
static_assert(test(0_bi, {5}));
static_assert(test(2_bi, {0, 0}));
static_assert(test(1_bi, {0, 5}));
static_assert(test(0_bi, {5, 0}));
static_assert(test(0_bi, {5, 5}));


constexpr auto test_backward(auto const index, containers::vector<int> const & v) -> bool {
	auto it = containers::find_last(v, 5);
	BOUNDED_ASSERT(index == containers::size(v) or *it == 5);
	return it - containers::begin(v) == index;
}
template<std::size_t size>
constexpr auto test_backward(auto const index, containers::c_array<int, size> && a) -> bool {
	return test_backward(index, containers::vector<int>(std::move(a)));
}

static_assert(test_backward(0_bi, {}));
static_assert(test_backward(1_bi, {0}));
static_assert(test_backward(0_bi, {5}));
static_assert(test_backward(2_bi, {0, 0}));
static_assert(test_backward(1_bi, {0, 5}));
static_assert(test_backward(0_bi, {5, 0}));
static_assert(test_backward(1_bi, {5, 5}));

} // namespace