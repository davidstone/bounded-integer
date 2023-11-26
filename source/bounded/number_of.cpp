// Copyright David Stone 2022.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

export module bounded.number_of;

import bounded.bounded_integer;
import bounded.integer;
import numeric_traits;

namespace bounded {

export template<isomorphic_to_integral T>
constexpr auto number_of = constant<numeric_traits::max_value<T>> - constant<numeric_traits::min_value<T>> + constant<1>;

} // namespace bounded
