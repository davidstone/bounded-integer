// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <bounded/detail/variant/variant.hpp>

#include "../../../test_assert.hpp"

namespace {

using namespace bounded::literal;

using thing_t = bounded::variant<int, short, long, char, int>;

static_assert(std::is_trivially_copy_constructible_v<thing_t>);
static_assert(std::is_trivially_move_constructible_v<thing_t>);
static_assert(std::is_trivially_copy_assignable_v<thing_t>);
static_assert(std::is_trivially_move_assignable_v<thing_t>);
static_assert(std::is_trivially_destructible_v<thing_t>);

template<bool expected, typename Index, typename... Ts>
constexpr bool assert_type_index_concepts() {
	static_assert(
		bounded::matches_exactly_one_type<
			Index,
			bounded::detail::types<Ts>...
		> == expected
	);
	static_assert(
		!bounded::variant_integer_index<
			bounded::detail::types<Index>,
			bounded::detail::types<Ts>...
		>
	);
	static_assert(
		bounded::unique_type_identifier<
			bounded::detail::types<Index>,
			bounded::detail::types<Ts>...
		> == expected
	);
	return true;
}

template<bool expected, auto index, typename... Ts>
constexpr bool assert_integer_index_concepts() {
	static_assert(
		!bounded::matches_exactly_one_type<
			bounded::constant_t<index>,
			bounded::detail::types<Ts>...
		>
	);
	static_assert(
		bounded::variant_integer_index<
			bounded::constant_t<index>,
			bounded::detail::types<Ts>...
		> == expected
	);
	static_assert(
		bounded::unique_type_identifier<
			bounded::constant_t<index>,
			bounded::detail::types<Ts>...
		> == expected
	);
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

static_assert(!bounded::is_constructible<thing_t, int>);
static_assert(bounded::is_constructible<thing_t, short>);
static_assert(bounded::is_constructible<thing_t, long>);
static_assert(bounded::is_constructible<thing_t, char>);

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

using empty_variant_t = bounded::variant<>;
static_assert(not std::is_default_constructible_v<empty_variant_t>);
static_assert(not bounded::is_constructible<empty_variant_t, bounded::constant_t<0>>);

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

struct non_comparable {
};

static_assert(!bounded::equality_comparable<non_comparable>);
static_assert(!bounded::equality_comparable<bounded::variant<non_comparable>>);
static_assert(!bounded::equality_comparable<bounded::variant<int, non_comparable>>);
static_assert(!bounded::equality_comparable<bounded::variant<non_comparable, int>>);
static_assert(!bounded::equality_comparable<bounded::variant<int, non_comparable, int>>);

}	// namespace

int main() {
	using bounded::construct_return;
	{
		using non_trivial_variant_t = bounded::variant<non_trivial>;

		static_assert(std::is_copy_constructible_v<non_trivial_variant_t>);
		static_assert(std::is_move_constructible_v<non_trivial_variant_t>);
		static_assert(std::is_copy_assignable_v<non_trivial_variant_t>);
		static_assert(std::is_move_assignable_v<non_trivial_variant_t>);
		static_assert(std::is_destructible_v<non_trivial_variant_t>);

		auto non_trivial_variant = non_trivial_variant_t(bounded::lazy_init, 0_bi, construct_return<non_trivial>);
		static_assert(non_trivial_variant.index() == 0_bi);
		non_trivial_variant = non_trivial_variant_t(bounded::lazy_init, 0_bi, construct_return<non_trivial>);
		// Silence self-assignment warning
		non_trivial_variant = *&non_trivial_variant;

		// TODO
		// auto const non_trivial_copy = non_trivial_variant;
		// assert(non_trivial_copy == non_trivial_variant);

		// TODO
		// auto const non_trivial_move = std::move(non_trivial_variant);
		// assert(non_trivial_copy == non_trivial_move);
	}
	
	{
		using non_copyable_variant_t = bounded::variant<non_copyable>;

		auto non_copyable_variant = non_copyable_variant_t(bounded::lazy_init, 0_bi, construct_return<non_copyable>);
		static_assert(non_copyable_variant.index() == 0_bi);
		static_assert(not std::is_copy_constructible_v<non_copyable_variant_t>);
		static_assert(not std::is_copy_assignable_v<non_copyable_variant_t>);
		static_assert(std::is_move_constructible_v<non_copyable_variant_t>);
		static_assert(std::is_move_assignable_v<non_copyable_variant_t>);
		static_assert(std::is_same_v<decltype(std::move(non_copyable_variant)[0_bi]), non_copyable &&>);
		{
			static_assert(!std::is_trivially_destructible_v<destructor_checker>);
			static_assert(!std::is_trivially_destructible_v<bounded::variant<destructor_checker>>);
			auto v = bounded::variant<destructor_checker>(bounded::lazy_init, 0_bi, construct_return<destructor_checker>);
		}
		BOUNDED_TEST(destructor_checker::destructed == 1U);
	}
}
