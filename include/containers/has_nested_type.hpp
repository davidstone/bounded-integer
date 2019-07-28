// Copyright David Stone 2015.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <type_traits>

#define CONTAINERS_MAKE_NESTED_TYPE_TEST(type_name) \
\
template<typename T> \
inline constexpr auto has_nested_type_impl_ ## type_name(typename std::remove_reference_t<T>::type_name *) noexcept { return true; } \
\
template<typename> \
inline constexpr auto has_nested_type_impl_ ## type_name(...) noexcept { return false; } \
\
template<typename T> \
inline constexpr auto has_nested_type_ ## type_name = detail::has_nested_type_impl_ ## type_name<T>(nullptr);

