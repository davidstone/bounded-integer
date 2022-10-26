// Copyright David Stone 2019.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <bounded/detail/copy_cv_ref.hpp>

#include <concepts>

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
