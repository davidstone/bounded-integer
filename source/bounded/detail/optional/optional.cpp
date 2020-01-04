// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <bounded/detail/optional/optional.hpp>

#include <bounded/integer.hpp>

#include <type_traits>

namespace {

static_assert(
	std::is_same<std::common_type_t<bounded::none_t, int>, bounded::optional<int>>{},
	"common_type with none_t first wrong."
);
static_assert(
	std::is_same<std::common_type_t<int, bounded::none_t>, bounded::optional<int>>{},
	"common_type with none_t last wrong."
);
static_assert(
	std::is_same<
		std::common_type_t<bounded::constant_t<1>, bounded::none_t, bounded::constant_t<5>, bounded::none_t>,
		bounded::optional<bounded::integer<1, 5>>
	>{},
	"common_type with bounded::integer and none_t wrong."
);

template<typename T>
constexpr auto test_optional() {
	using bounded::optional;
	static_assert(std::is_convertible_v<T, optional<T>>);
	static_assert(std::is_trivially_destructible_v<T> == std::is_trivially_destructible_v<optional<T>>);
	static_assert(std::is_trivially_copy_constructible_v<T> == std::is_trivially_copy_constructible_v<T>);
	static_assert(std::is_trivially_copy_assignable_v<T> == std::is_trivially_copy_assignable_v<T>);
	return true;
}

struct S {
	int member;
};

static_assert(test_optional<int>());
static_assert(test_optional<bounded::integer<0, 10>>());
static_assert(test_optional<decltype(bounded::integer(0))>());
constexpr auto test_insert() {
	auto op = bounded::optional<S>();
	bounded::insert(op, S{3});
	return op and op->member == 3;
}
static_assert(test_insert());

static_assert(*bounded::optional(5) == 5);

}	// namespace
