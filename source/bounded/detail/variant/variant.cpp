// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <bounded/detail/variant/variant.hpp>

#include "../../../test_assert.hpp"
#include "../../../test_int.hpp"

namespace {

using namespace bounded::literal;

using empty_variant_t = bounded::variant<>;
static_assert(not std::is_default_constructible_v<empty_variant_t>);
static_assert(not bounded::constructible_from<empty_variant_t, bounded::constant_t<0>>);
static_assert(std::is_empty_v<empty_variant_t>);

struct empty {};
static_assert(std::is_empty_v<bounded::variant<empty>>);

using thing_t = bounded::variant<int, short, long, char, int>;

static_assert(std::is_trivially_copy_constructible_v<thing_t>);
static_assert(std::is_trivially_move_constructible_v<thing_t>);
static_assert(std::is_trivially_copy_assignable_v<thing_t>);
static_assert(std::is_trivially_move_assignable_v<thing_t>);
static_assert(std::is_trivially_destructible_v<thing_t>);

template<bool expected, typename Index, typename... Ts>
constexpr bool assert_type_index_concepts() {
	static_assert(bounded::matches_exactly_one_type<Index, Ts...> == expected);
	static_assert(!bounded::variant_integer_index<bounded::detail::types<Index>, sizeof...(Ts)>);
	static_assert(bounded::unique_type_identifier<bounded::detail::types<Index>, Ts...> == expected);
	return true;
}

template<bool expected, auto index, typename... Ts>
constexpr bool assert_integer_index_concepts() {
	static_assert(!bounded::matches_exactly_one_type<bounded::constant_t<index>, Ts...>);
	static_assert(bounded::variant_integer_index<bounded::constant_t<index>, sizeof...(Ts)> == expected);
	static_assert(bounded::unique_type_identifier<bounded::constant_t<index>, Ts...> == expected);
	return true;
}

static_assert(assert_type_index_concepts<true, int, int>());
static_assert(assert_type_index_concepts<true, int, int, short>());
static_assert(assert_type_index_concepts<false, int, int, int>());
static_assert(assert_type_index_concepts<false, int, short>());
static_assert(assert_type_index_concepts<false, int>());

static_assert(assert_integer_index_concepts<true, 0, int>());
static_assert(assert_integer_index_concepts<true, 0, int, short>());
static_assert(assert_integer_index_concepts<true, 1, int, int>());
static_assert(assert_integer_index_concepts<false, 1, int>());
static_assert(assert_integer_index_concepts<false, 0>());

static_assert(!bounded::constructible_from<thing_t, int>);
static_assert(bounded::constructible_from<thing_t, short>);
static_assert(bounded::constructible_from<thing_t, long>);
static_assert(bounded::constructible_from<thing_t, char>);

static_assert(bounded::visit_with_index(
	thing_t(0_bi, 0),
	thing_t(0_bi, 0),
	bounded::detail::equality_visitor{}
));

static_assert(thing_t(0_bi, 0) == thing_t(0_bi, 0));
static_assert(thing_t(0_bi, 0) != thing_t(0_bi, 1));
static_assert(thing_t(0_bi, 0) != thing_t(1_bi, static_cast<short>(0)));
static_assert(thing_t(0_bi, 0) != thing_t(4_bi, 0));
static_assert(thing_t(0_bi, 0) != thing_t(4_bi, 1));

static_assert(thing_t(1_bi, static_cast<short>(5)) == thing_t(static_cast<short>(5)));

static_assert(holds_alternative(thing_t(1_bi, static_cast<short>(0)), bounded::detail::types<short>{}));
static_assert(holds_alternative(thing_t(2_bi, 0), bounded::detail::types<long>{}));
static_assert(holds_alternative(thing_t(3_bi, '\0'), bounded::detail::types<char>{}));

#if 0
// std::variant has these calls ill-formed (and that naturally happens with my
// implementation), but it seems like it should be legal.
static_assert(holds_alternative(
	thing_t(0_bi, 0),
	bounded::detail::types<int>{}
));

static_assert(holds_alternative(
	thing_t(4_bi, 0),
	bounded::detail::types<int>{}
));
#endif

constexpr auto index = 1_bi;
constexpr auto value = static_cast<short>(8);

constexpr auto thing = thing_t(index, value);
using thingy = decltype(thing[index]);

static_assert(std::is_same_v<thingy, short const &>);

static_assert(thing[index] == value);
static_assert(bounded::visit(thing, [](auto x) { return std::is_same_v<decltype(x), short>; }));

constexpr auto test_assignment_from_variant() {
	auto thing1 = thing_t(index, value);
	thing1 = thing_t(index, value);
	BOUNDED_TEST(thing1[index] == value);
	return true;
}
static_assert(test_assignment_from_variant());

constexpr auto test_assignment_from_value() {
	auto thing1 = thing_t(index, value);
	thing1 = -1L;
	BOUNDED_TEST(holds_alternative(thing1, bounded::detail::types<long>{}));
	BOUNDED_TEST(thing1[bounded::detail::types<long>{}] == -1L);
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

using non_copyable_variant_t = bounded::variant<non_copyable>;
static_assert(not std::is_copy_constructible_v<non_copyable_variant_t>);
static_assert(not std::is_copy_assignable_v<non_copyable_variant_t>);
static_assert(std::is_move_constructible_v<non_copyable_variant_t>);
static_assert(std::is_move_assignable_v<non_copyable_variant_t>);
static_assert(std::is_same_v<decltype(non_copyable_variant_t(bounded::lazy_init, 0_bi, bounded::construct_return<non_copyable>)[0_bi]), non_copyable &&>);

struct non_comparable {
};

static_assert(!bounded::equality_comparable<non_comparable>);
static_assert(!bounded::equality_comparable<bounded::variant<non_comparable>>);
static_assert(!bounded::equality_comparable<bounded::variant<int, non_comparable>>);
static_assert(!bounded::equality_comparable<bounded::variant<non_comparable, int>>);
static_assert(!bounded::equality_comparable<bounded::variant<int, non_comparable, int>>);

constexpr bool test_non_trivial() {
	using non_trivial_variant_t = bounded::variant<bounded::test_int>;
	auto a = non_trivial_variant_t(bounded::test_int(3));
	BOUNDED_TEST(a.index() == 0_bi);
	auto b = a;
	BOUNDED_TEST(a[0_bi].value() == 3);
	BOUNDED_TEST(a == b);
	return true;
}
static_assert(test_non_trivial());

} // namespace
