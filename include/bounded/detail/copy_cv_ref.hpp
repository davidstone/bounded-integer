// Copyright David Stone 2019.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <type_traits>

namespace bounded {
namespace detail {

template<typename Source, typename Target>
using copy_const = std::conditional_t<std::is_const_v<Source>, Target const, Target>;

template<typename Source, typename Target>
using copy_volatile = std::conditional_t<std::is_volatile_v<Source>, Target volatile, Target>;

template<typename Source, typename Target>
using copy_cv = copy_volatile<Source, copy_const<Source, Target>>;

template<typename Source, typename Target>
using copy_ref =
	std::conditional_t<std::is_lvalue_reference_v<Source>, Target &,
	std::conditional_t<std::is_rvalue_reference_v<Source>, Target &&,
	Target
>>;

template<typename Source, typename Target>
using copy_cv_ref = copy_ref<Source, copy_cv<std::remove_reference_t<Source>, Target>>;

} // namespace detail
} // namespace bounded
