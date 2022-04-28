// Copyright David Stone 2020.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <bounded/lazy_init.hpp>

namespace {

template<typename T>
auto function() -> T;
template<typename T>
constexpr auto object = []{ return T(); };

static_assert(bounded::construct_function_for<decltype(function<void>), void>);
static_assert(bounded::construct_function_for<decltype(object<void>), void>);

static_assert(!bounded::construct_function_for<decltype(function<int>), void>);
static_assert(!bounded::construct_function_for<decltype(object<int>), void>);

static_assert(!bounded::construct_function_for<decltype(function<void>), int>);
static_assert(!bounded::construct_function_for<decltype(object<void>), int>);

static_assert(bounded::construct_function_for<decltype(function<int>), int>);
static_assert(bounded::construct_function_for<decltype(object<int>), int>);

static_assert(bounded::construct_function_for<decltype(function<int>), short>);
static_assert(bounded::construct_function_for<decltype(object<int>), short>);

static_assert(bounded::construct_function_for<decltype(function<short>), int>);
static_assert(bounded::construct_function_for<decltype(object<short>), int>);

} // namespace