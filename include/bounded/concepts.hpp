// Copyright David Stone 2020.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <type_traits>

namespace bounded {

// TODO: File a bug against the standard? The standard version always returns a
// reference, which means it does not work with guaranteed copy elision.
template<typename T>
auto declval() -> T;

template<typename T> requires std::is_function_v<std::remove_cv_t<T>>
auto declval() -> T &&;

template<typename T, typename... Args>
concept constructible_from = requires { T(declval<Args>()...); };

template<typename From, typename To>
concept explicitly_convertible_to = constructible_from<To, From>;

} // namespace bounded
