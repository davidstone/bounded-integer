// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <bounded/detail/tuple.hpp>
#include <bounded/detail/literal.hpp>

#include <type_traits>

namespace {

using namespace bounded::literal;
BOUNDED_COMPARISON

static_assert(std::is_empty<bounded::tuple<>>::value);

struct empty {};

constexpr auto compare(empty, empty) noexcept {
	return bounded::strong_ordering_equal;
}

constexpr auto operator==(empty, empty) noexcept {
	return true;
}

static_assert(std::is_empty<bounded::tuple<empty>>::value);

static_assert(std::is_empty<bounded::tuple<void>>::value);

static_assert(std::is_empty<bounded::tuple<void, void, empty, void>>::value);

static_assert(std::is_empty<bounded::tuple<bounded::tuple<>>>::value);
static_assert(sizeof(bounded::tuple<int>) == sizeof(int));
// Make sure a tuple can handle multiple bases of the same type in the same position
static_assert(std::is_empty<bounded::tuple<empty, bounded::tuple<empty>>>::value);
static_assert(sizeof(bounded::tuple<int, bounded::tuple<int>>) == sizeof(int) * 2);

static_assert(sizeof(bounded::tuple<int, empty>) == sizeof(int));

constexpr auto a = bounded::tuple<int, empty, int>(0, empty{}, 5);

static_assert(a[0_bi] == 0);

static_assert(std::is_same_v<decltype(a[1_bi]), empty const &>);

static_assert(std::is_same<decltype(bounded::tuple<int>{}[0_bi]), int &&>::value);
static_assert(std::is_same<decltype(bounded::tuple<empty>{}[0_bi]), empty &&>::value);

constexpr bounded::tuple<int, char const *> b = { 2, "Hello" };
static_assert(b[0_bi] == b[0_bi]);

struct non_movable {
	non_movable() = default;
	non_movable(non_movable const &) = delete;
	non_movable(non_movable &&) = delete;
};

constexpr auto compare(non_movable const &, non_movable const &) noexcept {
	return bounded::strong_ordering_equal;
}
constexpr auto operator==(non_movable const &, non_movable const &) noexcept {
	return true;
}

static_assert(std::is_constructible<bounded::tuple<non_movable>>::value);
static_assert(std::is_constructible<bounded::tuple<non_movable>, std::piecewise_construct_t, bounded::tuple<>>::value);
static_assert(std::is_same<bounded::tuple_element<0, bounded::tuple<non_movable>>, non_movable>::value);
constexpr bounded::tuple<non_movable> non_movable_tuple = {};
static_assert(non_movable_tuple[0_bi] == non_movable{});

static_assert(a == a);
static_assert(b == b);
static_assert(non_movable_tuple == non_movable_tuple);
static_assert(a <= a);
static_assert(b <= b);
static_assert(non_movable_tuple <= non_movable_tuple);

constexpr auto d = bounded::tuple(0, empty{}, 4);
static_assert(a != d);
static_assert(a > d);

constexpr struct {
	constexpr auto operator()(float const x) const noexcept {
		return static_cast<int>(x + 1);
	}
	constexpr auto operator()(float const x, float const y) const noexcept {
		return static_cast<int>(x + y);
	}
	constexpr auto operator()(int const x) const noexcept {
		return static_cast<float>(x + 1);
	}
	constexpr auto operator()(int const x, int const y) const noexcept {
		return static_cast<float>(x + y);
	}
} increment_and_swap_types{};

constexpr auto input_tuple = bounded::make_tuple(0, 1.0F, 2, 3.0F, 4);
constexpr auto type_swapped = bounded::transform(increment_and_swap_types, input_tuple);
static_assert(std::is_same<decltype(type_swapped), bounded::tuple<float, int, float, int, float> const>::value);
static_assert(type_swapped == bounded::make_tuple(1.0F, 2, 3.0F, 4, 5.0F));
constexpr auto merged = bounded::transform(increment_and_swap_types, input_tuple, input_tuple);
static_assert(std::is_same<decltype(merged), bounded::tuple<float, int, float, int, float> const>::value);
static_assert(merged == bounded::make_tuple(0.0F, 2, 4.0F, 6, 8.0F));


}	// namespace
