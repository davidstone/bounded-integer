// Copyright David Stone 2019.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

export module bounded.copy_cv_ref;

import std_module;

namespace bounded {
namespace detail {

export template<typename Source, typename Target>
using copy_const = std::conditional_t<std::is_const_v<Source>, Target const, Target>;

export template<typename Source, typename Target>
using copy_volatile = std::conditional_t<std::is_volatile_v<Source>, Target volatile, Target>;

export template<typename Source, typename Target>
using copy_cv = copy_volatile<Source, copy_const<Source, Target>>;

export template<typename Source, typename Target>
using copy_ref =
	std::conditional_t<std::is_lvalue_reference_v<Source>, Target &,
	std::conditional_t<std::is_rvalue_reference_v<Source>, Target &&,
	Target
>>;

export template<typename Source, typename Target>
using copy_cv_ref = copy_ref<Source, copy_cv<std::remove_reference_t<Source>, Target>>;

} // namespace detail
} // namespace bounded

static_assert(std::same_as<bounded::detail::copy_cv_ref<int, long>, long>);
static_assert(std::same_as<bounded::detail::copy_cv_ref<int const, long>, long const>);
static_assert(std::same_as<bounded::detail::copy_cv_ref<int volatile, long>, long volatile>);
static_assert(std::same_as<bounded::detail::copy_cv_ref<int const volatile, long>, long const volatile>);
static_assert(std::same_as<bounded::detail::copy_cv_ref<int &, long>, long &>);
static_assert(std::same_as<bounded::detail::copy_cv_ref<int &&, long>, long &&>);
static_assert(std::same_as<bounded::detail::copy_cv_ref<int const &, long>, long const &>);
static_assert(std::same_as<bounded::detail::copy_cv_ref<int const &&, long>, long const &&>);
static_assert(std::same_as<bounded::detail::copy_cv_ref<int volatile &, long>, long volatile &>);
static_assert(std::same_as<bounded::detail::copy_cv_ref<int volatile &&, long>, long volatile &&>);
static_assert(std::same_as<bounded::detail::copy_cv_ref<int const volatile &, long>, long const volatile &>);
static_assert(std::same_as<bounded::detail::copy_cv_ref<int const volatile &&, long>, long const volatile &&>);
