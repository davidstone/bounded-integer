// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <bounded/detail/overload.hpp>

namespace {

struct int_function {
	constexpr auto operator()(int) const {
		return 3;
	}
};

struct pointer_function {
	[[maybe_unused]] constexpr auto operator()(int *) const {
		return 4;
	}
};

struct direct {
	constexpr auto operator()(int) const {
		return 2;
	}
	template<typename T>
	constexpr auto operator()(T) const {
		return 1;
	}
};

struct indirect : private int_function, private pointer_function {
	using int_function::operator();
	using pointer_function::operator();
};

static_assert(bounded::overload(int_function{}, [](int *) { return 0; })(10) == 3);
static_assert(bounded::overload(int_function{}, [](int const *) { return 0; })(10) == 3);
static_assert(bounded::overload([](auto) { return 0; })(10) == 0);
static_assert(direct{}(10) == 2);
static_assert(bounded::overload(direct{})(10) == 2);
static_assert(indirect{}(10) == 3);
static_assert(bounded::overload(indirect{})(10) == 3);
static_assert(bounded::overload(int_function{}, [](auto) { return 0; })(10) == 3);
static_assert(!std::is_invocable_v<decltype(bounded::overload(int_function{})), std::nullptr_t>);

} // namespace
