// Copyright David Stone 2015.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

module;

#include <std_module/prelude.hpp>
#include <climits>

export module bounded.size_of;

import bounded.integer;

namespace bounded {

export template<typename T>
constexpr auto size_of = constant<sizeof(T)>;

export constexpr auto char_bit = constant<CHAR_BIT>;

export template<typename T>
constexpr auto size_of_bits = size_of<T> * char_bit;

} // namespace bounded
