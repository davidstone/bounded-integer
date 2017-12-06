// Copyright David Stone 2017.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <bounded/detail/forward_declaration.hpp>

#include <type_traits>

namespace bounded {
namespace detail {

template<typename T>
struct is_poisoned_c : std::integral_constant<bool, !std::is_same<T, bool>{}> {};

template<auto minimum, auto maximum, typename policy, storage_type storage, bool poisoned>
struct is_poisoned_c<integer<minimum, maximum, policy, storage, poisoned>> : std::integral_constant<bool, poisoned> {};

template<typename T>
constexpr auto is_poisoned = is_poisoned_c<std::decay_t<T>>{};

}	// namespace detail
}	// namespace bounded
