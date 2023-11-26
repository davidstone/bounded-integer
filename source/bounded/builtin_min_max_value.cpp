// Copyright David Stone 2019.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

export module bounded.builtin_min_max_value;

import bounded.normalize;
import numeric_traits;

namespace bounded {

export template<numeric_traits::has_max_value T>
constexpr auto builtin_max_value = normalize<numeric_traits::max_value<T>>;

export template<numeric_traits::has_min_value T>
constexpr auto builtin_min_value = normalize<numeric_traits::min_value<T>>;

} // namespace bounded