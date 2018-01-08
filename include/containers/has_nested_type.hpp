// Copyright David Stone 2015.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <type_traits>

#define CONTAINERS_MAKE_NESTED_TYPE_TEST(type) \
\
template<typename T> \
constexpr auto has_nested_type_impl_ ## type(typename std::remove_reference_t<T>::type *) noexcept { return true; } \
\
template<typename> \
constexpr auto has_nested_type_impl_ ## type(...) noexcept { return false; } \
\
template<typename T> \
constexpr auto has_nested_type_ ## type = detail::has_nested_type_impl_ ##type<T>(nullptr);

