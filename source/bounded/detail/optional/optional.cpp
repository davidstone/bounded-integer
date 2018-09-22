// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <bounded/detail/optional/optional.hpp>

#include <bounded/integer.hpp>

#include <type_traits>

namespace {

template<typename T>
constexpr auto test_optional() {
	using bounded::optional;
	static_assert(std::is_convertible_v<T, optional<T>>);
	static_assert(std::is_trivially_destructible_v<T> == std::is_trivially_destructible_v<optional<T>>);
	static_assert(std::is_trivially_copy_constructible_v<T> == std::is_trivially_copy_constructible_v<T>);
	static_assert(std::is_trivially_copy_assignable_v<T> == std::is_trivially_copy_assignable_v<T>);
	return true;
}

static_assert(test_optional<int>());
static_assert(test_optional<bounded::integer<0, 10>>());
static_assert(test_optional<decltype(bounded::integer(0))>());

}	// namespace
