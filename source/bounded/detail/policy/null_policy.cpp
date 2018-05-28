// Copyright David Stone 2017.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <bounded/detail/policy/null_policy.hpp>
#include <bounded/detail/class.hpp>
#include <bounded/detail/comparison_mixed.hpp>

static_assert((static_cast<void>(bounded::null_policy{}.assignment(5, bounded::constant<0>, bounded::constant<10>)), true));
// This should not compile
// constexpr auto value2 = policy.assignment(15, bounded::constant<0>, bounded::constant<10>);

