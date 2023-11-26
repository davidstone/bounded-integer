// Copyright David Stone 2023.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

export module bounded.unsigned_builtin;

import numeric_traits;
import std_module;

namespace bounded {

export template<typename T>
concept unsigned_builtin = numeric_traits::unsigned_builtin<T> and !std::same_as<T, bool>;

} // namespace bounded
