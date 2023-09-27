// Copyright David Stone 2023.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

export module bounded.non_const;

import std_module;

namespace bounded {

export template<typename T>
concept non_const = !std::is_const_v<T>;

} // namespace bounded

static_assert(bounded::non_const<int>);
static_assert(!bounded::non_const<int const>);