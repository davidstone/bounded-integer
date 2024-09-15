// Copyright David Stone 2017.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

module;

#include <operators/forward.hpp>

export module bounded.construct_at;

import bounded.declval;
import bounded.lazy_init;
import bounded.non_const;

import std_module;

namespace bounded {

// https://github.com/llvm/llvm-project/issues/59513
struct construct_at_t {
	template<non_const T, construct_function_for<T> Function>
	static constexpr auto operator()(T & ref, Function && function) noexcept(noexcept(static_cast<T>(declval<Function>()()))) -> T & {
		return *::new(static_cast<void *>(std::addressof(ref))) T(std::invoke(OPERATORS_FORWARD(function)));
	}
};
export constexpr auto construct_at = construct_at_t();

} // namespace bounded

namespace {

struct empty {};

struct accepts_anything {
	template<typename T>
	explicit constexpr accepts_anything(T &&) {
		static_assert(std::same_as<T, int>);
	}
};

union u {
	constexpr u():
		e()
	{
	}
	empty e;
	accepts_anything a;
};

static_assert([]{
	auto x = u();
	bounded::construct_at(x.a, [] { return accepts_anything(5); });
	return true;
}());

auto make_int_noexcept() noexcept -> int;
static_assert(noexcept(bounded::construct_at(bounded::declval<int &>(), make_int_noexcept)));

auto make_int_throws() -> int;
static_assert(!noexcept(bounded::construct_at(bounded::declval<int &>(), make_int_throws)));

struct convert_to_int_throws {
	operator int() const;
};
auto make_convert_to_int_throws() noexcept -> convert_to_int_throws;
static_assert(!noexcept(bounded::construct_at(bounded::declval<int &>(), make_convert_to_int_throws)));

} // namespace
