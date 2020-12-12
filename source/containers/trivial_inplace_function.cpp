// Copyright David Stone 2020.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <containers/trivial_inplace_function.hpp>

#include "../test_assert.hpp"

namespace {

using const_function = containers::trivial_inplace_function<int() const, 24>;
using mutable_function = containers::trivial_inplace_function<int(), 24>;

static_assert(std::is_trivially_copyable_v<const_function>);
static_assert(std::is_trivially_copyable_v<mutable_function>);

static_assert(std::is_invocable_v<const_function const &>);
static_assert(std::is_invocable_v<const_function &>);

static_assert(!std::is_invocable_v<mutable_function const &>);
static_assert(std::is_invocable_v<mutable_function &>);

static_assert(!std::is_constructible_v<const_function, void(*)()>);
static_assert(!std::is_constructible_v<const_function, long(*)()>);

template<std::size_t size>
struct sized_function {
	containers::array<std::byte, size> a;
	auto operator()() const {
		return 1;
	}
};

static_assert(std::is_constructible_v<const_function, sized_function<24>>);
static_assert(!std::is_constructible_v<const_function, sized_function<25>>);

static_assert(sizeof(containers::trivial_inplace_function<void() const, 0>) == sizeof(void(*)()));

} // namespace

int main() {
	constexpr auto returns_five = const_function([] { return 5; });

	BOUNDED_TEST(returns_five() == 5);
	BOUNDED_TEST(returns_five() == 5);

	auto increments = mutable_function([x = 0]() mutable { return ++x; });

	BOUNDED_TEST(increments() == 1);
	BOUNDED_TEST(increments() == 2);

	using empty_function = containers::trivial_inplace_function<void() const, 0>;

	constexpr auto empty_lambda = []{};
	using empty_lambda_t = decltype(empty_lambda);
	static_assert(std::is_empty_v<empty_lambda_t>);
	static_assert(std::is_constructible_v<empty_function, empty_lambda_t>);

	constexpr auto empty_capture_lambda = [=]{};
	using empty_capture_lambda_t = decltype(empty_capture_lambda);
	static_assert(std::is_empty_v<empty_capture_lambda_t>);
	static_assert(std::is_constructible_v<empty_function, empty_capture_lambda_t>);

	constexpr auto non_empty_lambda = [x = 0]{ static_cast<void>(x); };
	using non_empty_lambda_t = decltype(non_empty_lambda);
	static_assert(!std::is_empty_v<non_empty_lambda_t>);
	static_assert(!std::is_constructible_v<empty_function, non_empty_lambda_t>);
}