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
struct is_bounded_integer_c : std::false_type {};

template<auto minimum, auto maximum, typename overflow_policy, bool poisoned>
struct is_bounded_integer_c<integer<minimum, maximum, overflow_policy, poisoned>> : std::true_type {};

}	// namespace detail

template<typename T>
constexpr auto is_bounded_integer = detail::is_bounded_integer_c<std::decay_t<T>>{};

}	// namespace bounded
