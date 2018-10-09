// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <bounded/detail/variant/variant.hpp>

namespace {

using namespace bounded::literal;

constexpr auto index = 1_bi;
constexpr auto value = static_cast<short>(8);

using thing_t = bounded::variant<int, short, long, char, int>;
constexpr auto thing = thing_t(index, value);
using thingy = decltype(thing[index]);

static_assert(std::is_same_v<thingy, short const &>);

static_assert(thing[index] == value);
static_assert(bounded::visit(thing, [](auto x) { return std::is_same_v<decltype(x), short>; }));

constexpr auto test_assignment() {
	thing_t thing1(index, value);
	thing1 = thing_t(index, value);
	return thing1;
}

static_assert(test_assignment()[index] == value);

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
	auto non_trivial_variant = non_trivial_variant_t(0_bi);
	assert(non_trivial_variant.index() == 0_bi);
	non_trivial_variant = non_trivial_variant_t(0_bi);
	// Silence self-assignment warning
	non_trivial_variant = *&non_trivial_variant;
	
	
	using non_copyable_variant_t = bounded::variant<non_copyable>;
	auto non_copyable_variant = non_copyable_variant_t(0_bi);
	assert(non_copyable_variant.index() == 0_bi);
	static_assert(not std::is_copy_constructible<non_copyable_variant_t>{});
	static_assert(not std::is_copy_assignable<non_copyable_variant_t>{});
	static_assert(std::is_move_constructible<non_copyable_variant_t>{});
	static_assert(std::is_move_assignable<non_copyable_variant_t>{});
	static_assert(std::is_same<decltype(std::move(non_copyable_variant)[0_bi]), non_copyable &&>{});
	{
		static_assert(!std::is_trivially_destructible_v<destructor_checker>);
		static_assert(!std::is_trivially_destructible_v<bounded::variant<destructor_checker>>);
		auto v = bounded::variant<destructor_checker>(0_bi);
	}
	assert(destructor_checker::destructed == 1U);
}
