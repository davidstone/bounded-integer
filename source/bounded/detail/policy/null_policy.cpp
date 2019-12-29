// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <bounded/detail/policy/null_policy.hpp>
#include <bounded/detail/class.hpp>

namespace {
static_assert(bounded::null_policy::assignment(bounded::constant<5>, bounded::constant<0>, bounded::constant<10>) == bounded::constant<5>);
// This should not compile
// constexpr auto value2 = bounded::null_policy::assignment(15, bounded::constant<0>, bounded::constant<10>);

} // namespace
