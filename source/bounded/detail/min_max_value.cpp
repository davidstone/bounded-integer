// Copyright David Stone 2019.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <bounded/detail/min_max_value.hpp>

namespace {

template<typename T>
constexpr bool check_type() {
	static_assert(bounded::min_value<T> == std::numeric_limits<T>::min());
	static_assert(bounded::max_value<T> == std::numeric_limits<T>::max());
	static_assert(bounded::max_value<T> != 0);
	return true;
}

template<typename T>
constexpr bool check_cv_ref_qualifiers() {
	static_assert(check_type<T>());
	static_assert(check_type<T const>());
	static_assert(check_type<T volatile>());
	static_assert(check_type<T const volatile>());
	return true;
}

static_assert(check_cv_ref_qualifiers<signed char>());
static_assert(check_cv_ref_qualifiers<short>());
static_assert(check_cv_ref_qualifiers<int>());
static_assert(check_cv_ref_qualifiers<long>());
static_assert(check_cv_ref_qualifiers<long long>());

static_assert(check_cv_ref_qualifiers<unsigned char>());
static_assert(check_cv_ref_qualifiers<unsigned short>());
static_assert(check_cv_ref_qualifiers<unsigned int>());
static_assert(check_cv_ref_qualifiers<unsigned long>());
static_assert(check_cv_ref_qualifiers<unsigned long long>());

#if defined BOUNDED_DETAIL_HAS_128_BIT
static_assert(check_cv_ref_qualifiers<bounded::detail::int128_t>());
static_assert(check_cv_ref_qualifiers<bounded::detail::uint128_t>());
#endif

} // namespace