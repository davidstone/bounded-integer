// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <bounded/detail/overload.hpp>
#include <bounded/detail/variant/variant.hpp>

namespace {

constexpr auto variant = bounded::variant<int, int *>(bounded::detail::types<int>{}, 5);

struct int_visitor {
	constexpr auto operator()(int) const {
		return 3;
	}
};

static_assert(bounded::visit(variant, bounded::overload(int_visitor{}, [](int *) { return 0; })) == 3);
static_assert(bounded::visit(variant, bounded::overload(int_visitor{}, [](int const *) { return 0; })) == 3);
static_assert(!std::is_invocable_v<bounded::visit_t, decltype(variant), int_visitor>);
static_assert(!std::is_invocable_v<bounded::visit_t, decltype(variant), decltype(bounded::overload(int_visitor{}))>);

} // namespace
