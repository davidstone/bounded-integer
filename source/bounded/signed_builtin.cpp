// Copyright David Stone 2023.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

export module bounded.signed_builtin;

import numeric_traits;

namespace bounded {

export template<typename T>
concept signed_builtin = numeric_traits::signed_builtin<T>;

} // namespace bounded
