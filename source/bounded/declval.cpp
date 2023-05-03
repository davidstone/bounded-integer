// Copyright David Stone 2023.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

export module bounded.declval;

import std_module;

namespace bounded {

// TODO: File a bug against the standard? The standard version always returns a
// reference, which means it does not work with guaranteed copy elision.
export template<typename T>
auto declval() noexcept -> T;

export template<typename T> requires (std::is_function_v<std::remove_cv_t<T>> or std::is_array_v<std::remove_cv_t<T>>)
auto declval() noexcept -> T &&;

} // namespace bounded
