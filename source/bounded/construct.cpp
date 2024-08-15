// Copyright David Stone 2017.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

module;

#include <operators/forward.hpp>

export module bounded.construct;

import bounded.concepts;

import std_module;

namespace bounded {

// TODO: Use lambda https://github.com/llvm/llvm-project/issues/59513
template<typename T>
struct construct_t {
	template<typename... Args> requires constructible_from<T, Args &&...>
	static constexpr auto operator()(Args && ... args) noexcept(std::is_nothrow_constructible_v<T, Args &&...>) -> T {
		return T(OPERATORS_FORWARD(args)...);
	}
};
export template<typename T>
constexpr auto construct = construct_t<T>();

} // namespace bounded

namespace {

static_assert(bounded::construct<int>() == 0);
static_assert(bounded::construct<int>(2) == 2);
static_assert(noexcept(bounded::construct<int>()));

struct not_noexcept {
	not_noexcept() {}
};

static_assert(!noexcept(bounded::construct<not_noexcept>()));

struct convert_to_int_throws {
	operator int() const;
};

static_assert(!noexcept(bounded::construct<int>(convert_to_int_throws())));

} // namespace