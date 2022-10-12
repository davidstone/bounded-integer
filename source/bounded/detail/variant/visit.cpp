// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <bounded/overload.hpp>
#include <bounded/detail/variant/variant.hpp>

namespace {

constexpr auto variant = bounded::variant<int, int *>(5);

constexpr auto int_visitor = [](int) {
	return 3;
};

struct special {
	static constexpr auto operator()(int) {
		return 2;
	}
	static constexpr auto operator()(auto) {
		return 1;
	}
};

static_assert(
	bounded::detail::rotate_transform(
		[](auto f) { return f(); },
		[] { return 3; }
	) == 3
);
static_assert(
	bounded::detail::rotate_transform(
		[](auto f, int x) { return f(x); },
		7,
		[](int x) { return x; }
	) == 7
);

static_assert(bounded::visit(bounded::variant<int>(5), int_visitor) == 3);

static_assert(
	bounded::detail::rotate_transform(
		[](auto f, auto... values) { return f(values...); },
		1,
		2,
		3,
		[](int x, int y, int z) { return x * 100 + y * 10 + z; }
	) == 123
);


static_assert(bounded::visit(variant, bounded::overload(int_visitor, [](int *) { return 0; })) == 3);
static_assert(bounded::visit(variant, bounded::overload(int_visitor, [](int const *) { return 0; })) == 3);
static_assert(bounded::visit(variant, bounded::overload([](auto) { return 0; })) == 0);
static_assert(bounded::visit(variant, special{}) == 2);
static_assert(bounded::visit(variant, bounded::overload(int_visitor, [](auto) { return 0; })) == 3);
static_assert(bounded::visit(bounded::variant<int>(1), [](int && x) { return x; }) == 1);

static_assert(!std::is_invocable_v<decltype(bounded::visit), decltype(variant), decltype(int_visitor)>);
static_assert(!std::is_invocable_v<decltype(bounded::visit), decltype(variant), decltype(bounded::overload(int_visitor))>);

} // namespace
