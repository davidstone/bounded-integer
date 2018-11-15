// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <bounded/detail/variant/variant.hpp>

#include <cassert>

namespace {

using namespace bounded::literal;

using thing_t = bounded::variant<int, short, long, char, int>;

static_assert(!std::is_constructible_v<thing_t, int>);
static_assert(std::is_constructible_v<thing_t, short>);
static_assert(std::is_constructible_v<thing_t, long>);
static_assert(std::is_constructible_v<thing_t, char>);

static_assert(thing_t(std::in_place, 0_bi, 0) == thing_t(std::in_place, 0_bi, 0));
static_assert(thing_t(std::in_place, 0_bi, 0) != thing_t(std::in_place, 0_bi, 1));
static_assert(thing_t(std::in_place, 0_bi, 0) != thing_t(std::in_place, 1_bi, static_cast<short>(0)));
static_assert(thing_t(std::in_place, 0_bi, 0) != thing_t(std::in_place, 4_bi, 0));
static_assert(thing_t(std::in_place, 0_bi, 0) != thing_t(std::in_place, 4_bi, 1));

static_assert(thing_t(std::in_place, 1_bi, static_cast<short>(5)) == thing_t(static_cast<short>(5)));

static_assert(holds_alternative(thing_t(std::in_place, 1_bi, static_cast<short>(0)), bounded::detail::types<short>{}));
static_assert(holds_alternative(thing_t(std::in_place, 2_bi, 0), bounded::detail::types<long>{}));
static_assert(holds_alternative(thing_t(std::in_place, 3_bi, '\0'), bounded::detail::types<char>{}));

#if 0
// std::variant has these calls ill-formed (and that naturally happens with my
// implementation), but it seems like it should be legal.
static_assert(holds_alternative(thing_t(std::in_place, 0_bi, 0), bounded::detail::types<int>{}));
static_assert(holds_alternative(thing_t(std::in_place, 4_bi, 0), bounded::detail::types<int>{}));
#endif

constexpr auto index = 1_bi;
constexpr auto value = static_cast<short>(8);

constexpr auto thing = thing_t(std::in_place, index, value);
using thingy = decltype(thing[index]);

static_assert(std::is_same_v<thingy, short const &>);

static_assert(thing[index] == value);
static_assert(bounded::visit(thing, [](auto x) { return std::is_same_v<decltype(x), short>; }));

constexpr auto test_assignment_from_variant() {
	auto thing1 = thing_t(std::in_place, index, value);
	thing1 = thing_t(std::in_place, index, value);
	assert(thing1[index] == value);
	return true;
}
static_assert(test_assignment_from_variant());

constexpr auto test_assignment_from_value() {
	auto thing1 = thing_t(std::in_place, index, value);
	thing1 = -1L;
	assert(holds_alternative(thing1, bounded::detail::types<long>{}));
	assert(thing1[bounded::detail::types<long>{}] == -1L);
	return true;
}
static_assert(test_assignment_from_value());

using empty_variant_t = bounded::variant<>;
static_assert(not std::is_default_constructible_v<empty_variant_t>);
static_assert(not std::is_constructible_v<empty_variant_t, bounded::constant_t<0>>);

struct non_trivial {
	constexpr non_trivial() {}
	constexpr non_trivial(non_trivial const &) {}
	constexpr non_trivial & operator=(non_trivial const &) { return *this; }
	~non_trivial() {}
};

struct non_copyable {
	constexpr non_copyable() = default;
	constexpr non_copyable(non_copyable const &) = delete;
	constexpr non_copyable(non_copyable &&) = default;
	constexpr non_copyable & operator=(non_copyable const &) = delete;
	constexpr non_copyable & operator=(non_copyable &&) = default;
};

struct destructor_checker {
	static inline auto destructed = 0U;
	~destructor_checker() {
		++destructed;
	}
};

}	// namespace

int main() {
	using non_trivial_variant_t = bounded::variant<non_trivial>;
	auto non_trivial_variant = non_trivial_variant_t(std::in_place, 0_bi);
	assert(non_trivial_variant.index() == 0_bi);
	non_trivial_variant = non_trivial_variant_t(std::in_place, 0_bi);
	// Silence self-assignment warning
	non_trivial_variant = *&non_trivial_variant;
	
	
	using non_copyable_variant_t = bounded::variant<non_copyable>;
	auto non_copyable_variant = non_copyable_variant_t(std::in_place, 0_bi);
	assert(non_copyable_variant.index() == 0_bi);
	static_assert(not std::is_copy_constructible_v<non_copyable_variant_t>);
	static_assert(not std::is_copy_assignable_v<non_copyable_variant_t>);
	static_assert(std::is_move_constructible_v<non_copyable_variant_t>);
	static_assert(std::is_move_assignable_v<non_copyable_variant_t>);
	static_assert(std::is_same_v<decltype(std::move(non_copyable_variant)[0_bi]), non_copyable &&>);
	{
		static_assert(!std::is_trivially_destructible_v<destructor_checker>);
		static_assert(!std::is_trivially_destructible_v<bounded::variant<destructor_checker>>);
		auto v = bounded::variant<destructor_checker>(std::in_place, 0_bi);
	}
	assert(destructor_checker::destructed == 1U);
}
