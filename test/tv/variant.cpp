// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

module;

#include <bounded/assert.hpp>

export module tv.test.variant;

import tv.is_valid_index;
import tv.overload;
import tv.variant;

import bounded;
import bounded.test_int;
import std_module;

namespace {

using namespace bounded::literal;

using empty_variant_t = tv::variant<>;
static_assert(not bounded::default_constructible<empty_variant_t>);
static_assert(not bounded::constructible_from<empty_variant_t, bounded::constant_t<0>>);

struct empty {};
static_assert(std::is_empty_v<tv::variant<empty>>);

using thing_t = tv::variant<int, short, long, char, int>;

static_assert(bounded::trivially_copy_constructible<thing_t>);
static_assert(bounded::trivially_move_constructible<thing_t>);
static_assert(bounded::trivially_copy_assignable<thing_t>);
static_assert(bounded::trivially_move_assignable<thing_t>);
static_assert(bounded::trivially_destructible<thing_t>);

struct non_trivial {
	non_trivial();
	non_trivial(non_trivial &&);
	non_trivial(non_trivial const &);
	auto operator=(non_trivial &&) -> non_trivial &;
	auto operator=(non_trivial const &) -> non_trivial &;
	~non_trivial();
};

static_assert(!bounded::trivially_copy_constructible<tv::variant<non_trivial>>);
static_assert(!bounded::trivially_move_constructible<tv::variant<non_trivial>>);
static_assert(!bounded::trivially_copy_assignable<tv::variant<non_trivial>>);
static_assert(!bounded::trivially_move_assignable<tv::variant<non_trivial>>);
static_assert(!bounded::trivially_destructible<tv::variant<non_trivial>>);

static_assert(!bounded::constructible_from<thing_t, int>);
static_assert(bounded::constructible_from<thing_t, short>);
static_assert(bounded::constructible_from<thing_t, long>);
static_assert(bounded::constructible_from<thing_t, char>);

static_assert(thing_t(0_bi, 0) == thing_t(0_bi, 0));
static_assert(thing_t(0_bi, 0) != thing_t(0_bi, 1));
static_assert(thing_t(0_bi, 0) != thing_t(1_bi, static_cast<short>(0)));
static_assert(thing_t(0_bi, 0) != thing_t(4_bi, 0));
static_assert(thing_t(0_bi, 0) != thing_t(4_bi, 1));

static_assert(thing_t(1_bi, static_cast<short>(5)) == thing_t(static_cast<short>(5)));

static_assert(thing_t(0_bi, 0).index() == 0_bi);
static_assert(thing_t(1_bi, static_cast<short>(0)).index() == 1_bi);
static_assert(thing_t(2_bi, 0).index() == 2_bi);
static_assert(thing_t(3_bi, '\0').index() == 3_bi);
static_assert(thing_t(4_bi, 0).index() == 4_bi);

constexpr auto index = 1_bi;
constexpr auto value = static_cast<short>(8);

constexpr auto thing = thing_t(index, value);
using thingy = decltype(thing[index]);

static_assert(std::same_as<thingy, short const &>);

static_assert(thing[index] == value);

constexpr auto test_assignment_from_variant() {
	auto thing1 = thing_t(index, value);
	thing1 = thing_t(index, value);
	BOUNDED_ASSERT(thing1[index] == value);
	return true;
}
static_assert(test_assignment_from_variant());

constexpr auto test_assignment_from_value() {
	auto thing1 = thing_t(index, value);
	thing1 = -1L;
	BOUNDED_ASSERT(thing1.index() == bounded::type<long>);
	BOUNDED_ASSERT(thing1[bounded::type<long>] == -1L);
	return true;
}
static_assert(test_assignment_from_value());

struct non_copyable {
	constexpr non_copyable() = default;
	constexpr non_copyable(non_copyable const &) = delete;
	constexpr non_copyable(non_copyable &&) = default;
	constexpr non_copyable & operator=(non_copyable const &) = delete;
	constexpr non_copyable & operator=(non_copyable &&) = default;
};

using non_copyable_variant_t = tv::variant<non_copyable>;
static_assert(not bounded::copy_constructible<non_copyable_variant_t>);
static_assert(not bounded::copy_assignable<non_copyable_variant_t>);
static_assert(bounded::move_constructible<non_copyable_variant_t>);
static_assert(bounded::move_assignable<non_copyable_variant_t>);
static_assert(std::same_as<decltype(non_copyable_variant_t(bounded::lazy_init, 0_bi, bounded::construct<non_copyable>)[0_bi]), non_copyable &&>);

struct non_comparable {
};

static_assert(!bounded::equality_comparable<non_comparable>);
static_assert(!bounded::equality_comparable<tv::variant<non_comparable>>);
static_assert(!bounded::equality_comparable<tv::variant<int, non_comparable>>);
static_assert(!bounded::equality_comparable<tv::variant<non_comparable, int>>);
static_assert(!bounded::equality_comparable<tv::variant<int, non_comparable, int>>);

constexpr bool test_non_trivial() {
	using non_trivial_variant_t = tv::variant<bounded_test::integer>;
	auto a = non_trivial_variant_t(bounded_test::integer(3));
	BOUNDED_ASSERT(a.index() == 0_bi);
	auto b = a;
	BOUNDED_ASSERT(a[0_bi].value() == 3);
	BOUNDED_ASSERT(a == b);
	return true;
}
static_assert(test_non_trivial());

static_assert(std::same_as<
	decltype(bounded::declval<tv::variant<int> &>().emplace(0_bi, [] { return 0; })),
	int &
>);

static_assert(bounded::convertible_to<int, tv::variant<int, unsigned>>);
static_assert(!bounded::convertible_to<short, tv::variant<int, unsigned>>);
static_assert(!bounded::convertible_to<int, tv::variant<int, int>>);

} // namespace