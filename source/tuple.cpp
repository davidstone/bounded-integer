// Copyright David Stone 2016.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <containers/tuple.hpp>

#include <type_traits>

namespace {

using namespace bounded::literal;

static_assert(std::is_empty<containers::tuple<>>::value);

struct empty {};

constexpr auto operator==(empty const &, empty const &) noexcept {
	return std::true_type{};
}
constexpr auto operator<(empty const &, empty const &) noexcept {
	return std::false_type{};
}

static_assert(std::is_empty<containers::tuple<empty>>::value);

static_assert(std::is_empty<containers::tuple<void>>::value);

static_assert(std::is_empty<containers::tuple<void, void, empty, void>>::value);

static_assert(std::is_empty<containers::tuple<containers::tuple<>>>::value);
static_assert(sizeof(containers::tuple<int>) == sizeof(int));
// Make sure a tuple can handle multiple bases of the same type in the same position
static_assert(std::is_empty<containers::tuple<empty, containers::tuple<empty>>>::value);
static_assert(sizeof(containers::tuple<int, containers::tuple<int>>) == sizeof(int) * 2);

static_assert(sizeof(containers::tuple<int, empty>) == sizeof(int));

constexpr auto a = containers::tuple<int, empty, int>(0, empty{}, 5);

static_assert(a[0_bi] == 0);

static_assert(std::is_same<decltype(a[1_bi]), empty const &>::value);

static_assert(std::is_same<decltype(containers::tuple<int>{}[0_bi]), int &&>::value);
static_assert(std::is_same<decltype(containers::tuple<empty>{}[0_bi]), empty &&>::value);

constexpr containers::tuple<int, char const *> b = { 2, "Hello" };
static_assert(b[0_bi] == b[0_bi]);

struct non_movable {
	non_movable() = default;
	non_movable(non_movable const &) = delete;
	non_movable(non_movable &&) = delete;
};

constexpr auto operator==(non_movable const &, non_movable const &) noexcept {
	return std::true_type{};
}
constexpr auto operator<(non_movable const &, non_movable const &) noexcept {
	return std::false_type{};
}

static_assert(std::is_constructible<containers::tuple<non_movable>>::value);
static_assert(std::is_constructible<containers::tuple<non_movable>, std::piecewise_construct_t, containers::tuple<>>::value);
static_assert(std::is_same<containers::tuple_element<0, containers::tuple<non_movable>>, non_movable>::value);
constexpr containers::tuple<non_movable> non_movable_tuple = {};
static_assert(non_movable_tuple[0_bi] == non_movable{});

static_assert(a == a);
static_assert(b == b);
static_assert(non_movable_tuple == non_movable_tuple);
static_assert(a <= a);
static_assert(b <= b);
static_assert(non_movable_tuple <= non_movable_tuple);

constexpr auto d = containers::make_tuple(0, empty{}, 4);
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
} increment_and_swap_types;

constexpr auto input_tuple = containers::make_tuple(0, 1.0F, 2, 3.0F, 4);
constexpr auto type_swapped = containers::transform(increment_and_swap_types, input_tuple);
static_assert(std::is_same<decltype(type_swapped), containers::tuple<float, int, float, int, float> const>::value);
static_assert(type_swapped == containers::make_tuple(1.0F, 2, 3.0F, 4, 5.0F));
constexpr auto merged = containers::transform(increment_and_swap_types, input_tuple, input_tuple);
static_assert(std::is_same<decltype(merged), containers::tuple<float, int, float, int, float> const>::value);
static_assert(merged == containers::make_tuple(0.0F, 2, 4.0F, 6, 8.0F));


}	// namespace
