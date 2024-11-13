// Copyright David Stone 2020.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <std_module/prelude.hpp>
#include <catch2/catch_test_macros.hpp>

import containers.trivial_inplace_function;

import containers.array;

import bounded;
import std_module;

namespace {
using namespace bounded::literal;

using empty_const_function = containers::trivial_inplace_function<int() const, 0>;
using const_function = containers::trivial_inplace_function<int() const, 24>;
using empty_mutable_function = containers::trivial_inplace_function<int(), 0>;
using mutable_function = containers::trivial_inplace_function<int(), 24>;

static_assert(std::is_trivially_copyable_v<empty_const_function>);
static_assert(std::is_trivially_copyable_v<const_function>);
static_assert(std::is_trivially_copyable_v<empty_mutable_function>);
static_assert(std::is_trivially_copyable_v<mutable_function>);

static_assert(std::invocable<empty_const_function const &>);
static_assert(std::invocable<const_function const &>);
static_assert(std::invocable<empty_const_function &>);
static_assert(std::invocable<const_function &>);

static_assert(!std::invocable<empty_mutable_function const &>);
static_assert(!std::invocable<mutable_function const &>);
static_assert(std::invocable<empty_mutable_function &>);
static_assert(std::invocable<mutable_function &>);

constexpr auto normal_function() -> int {
	return 2;
}
static_assert(empty_const_function(normal_function)() == 2);
static_assert(bounded::constructible_from<const_function, int(*)()>);
static_assert(empty_mutable_function(normal_function)() == 2);
static_assert(bounded::constructible_from<const_function, int(*)()>);

static_assert(!bounded::constructible_from<empty_const_function, void(*)()>);
static_assert(!bounded::constructible_from<const_function, void(*)()>);
static_assert(!bounded::constructible_from<empty_const_function, long(*)()>);
static_assert(!bounded::constructible_from<const_function, long(*)()>);

template<std::size_t size>
struct sized_function {
	[[no_unique_address]] containers::array<std::byte, bounded::constant<size>> a;
	auto operator()() const {
		return 1;
	}
};

static_assert(bounded::constructible_from<empty_const_function, sized_function<0>>);
static_assert(bounded::constructible_from<const_function, sized_function<0>>);
static_assert(bounded::constructible_from<empty_mutable_function, sized_function<0>>);
static_assert(bounded::constructible_from<mutable_function, sized_function<0>>);

static_assert(!bounded::constructible_from<empty_const_function, sized_function<24>>);
static_assert(bounded::constructible_from<const_function, sized_function<24>>);
static_assert(!bounded::constructible_from<empty_mutable_function, sized_function<24>>);
static_assert(bounded::constructible_from<mutable_function, sized_function<24>>);

static_assert(!bounded::constructible_from<empty_const_function, sized_function<25>>);
static_assert(!bounded::constructible_from<const_function, sized_function<25>>);
static_assert(!bounded::constructible_from<empty_mutable_function, sized_function<25>>);
static_assert(!bounded::constructible_from<mutable_function, sized_function<25>>);

static_assert(sizeof(containers::trivial_inplace_function<void() const, 0>) == sizeof(void(*)()));

constexpr auto returns_five = empty_const_function([] { return 5; });
static_assert(returns_five() == 5);

[[maybe_unused]] void test_lambdas() {
	constexpr auto empty_capture_lambda = [=]{};
	using empty_capture_lambda_t = decltype(empty_capture_lambda);
	static_assert(std::is_empty_v<empty_capture_lambda_t>);
	static_assert(!bounded::trivially_default_constructible<empty_capture_lambda_t>);
	static_assert(!bounded::constructible_from<empty_const_function, empty_capture_lambda_t>);

	constexpr auto non_empty_lambda = [x = 0]{ static_cast<void>(x); };
	using non_empty_lambda_t = decltype(non_empty_lambda);
	static_assert(!std::is_empty_v<non_empty_lambda_t>);
	static_assert(!bounded::constructible_from<empty_const_function, non_empty_lambda_t>);
}

using tombstone_traits = bounded::tombstone_traits<
	containers::trivial_inplace_function<void() const, 0>
>;
static_assert(tombstone_traits::spare_representations == 1_bi);
static_assert(tombstone_traits::index(tombstone_traits::make(0_bi)) == 0_bi);
static_assert(tombstone_traits::index([]{}) == -1_bi);

constexpr auto big_returns_five = const_function([] { return 5; });

TEST_CASE("trivial_inplace_function lambda", "[trivial_inplace_function]") {
	CHECK(big_returns_five() == 5);

	auto increments = mutable_function([x = 0]() mutable { return ++x; });

	CHECK(increments() == 1);
	CHECK(increments() == 2);
}

TEST_CASE("trivial_inplace_function const const function pointer", "[trivial_inplace_function]") {
	auto const f = const_function(normal_function);
	CHECK(f() == 2);
}

TEST_CASE("trivial_inplace_function const mutable function pointer", "[trivial_inplace_function]") {
	auto f = const_function(normal_function);
	CHECK(f() == 2);
}

TEST_CASE("trivial_inplace_function mutable function pointer", "[trivial_inplace_function]") {
	auto f = mutable_function(normal_function);
	CHECK(f() == 2);
}

} // namespace
