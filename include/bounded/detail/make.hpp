// Copyright David Stone 2015.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <bounded/detail/basic_numeric_limits.hpp>
#include <bounded/detail/class.hpp>
#include <bounded/detail/is_bounded_integer.hpp>
#include <bounded/detail/underlying_type.hpp>
#include <bounded/detail/policy/modulo_policy.hpp>
#include <bounded/detail/policy/null_policy.hpp>
#include <cstdint>
#include <type_traits>

namespace bounded {
namespace detail {

template<typename T>
struct equivalent_overflow_policy_c {
	using type = std::conditional_t<std::is_unsigned<T>::value, modulo_policy, null_policy>;
};
template<intmax_t minimum, intmax_t maximum, typename overflow_policy, storage_type storage>
struct equivalent_overflow_policy_c<integer<minimum, maximum, overflow_policy, storage>> {
	using type = overflow_policy;
};

template<typename T>
using equivalent_overflow_policy = typename equivalent_overflow_policy_c<std::decay_t<T>>::type;

template<typename T, typename overflow_policy, storage_type storage, bool is_specialized = basic_numeric_limits<T>::is_specialized>
struct equivalent_type_c;

template<typename T, typename overflow_policy, storage_type storage>
struct equivalent_type_c<T, overflow_policy, storage, true> {
	using type = integer<
		static_cast<std::intmax_t>(basic_numeric_limits<T>::min()),
		static_cast<std::intmax_t>(basic_numeric_limits<T>::max()),
		overflow_policy,
		storage,
		std::is_integral<T>::value
	>;
};

template<typename T, typename overflow_policy, storage_type storage>
struct equivalent_type_c<T, overflow_policy, storage, false> {
	static_assert(std::is_enum<T>::value);
	using type = integer<
		basic_numeric_limits<std::underlying_type_t<T>>::min(),
		basic_numeric_limits<std::underlying_type_t<T>>::max(),
		overflow_policy,
		storage,
		true
	>;
};

}	// namespace detail

template<
	typename T,
	typename overflow_policy = detail::equivalent_overflow_policy<T>,
	storage_type storage = storage_type::fast
>
using equivalent_type = typename detail::equivalent_type_c<std::decay_t<T>, overflow_policy, storage>::type;

// This somewhat strange looking set of default arguments allows the following:
//
// 1) The first parameter is defaulted to void. The common case for this
// function is to convert a built-in integer type to a bounded::integer. The
// bounded::integer analog to built-in integers is a bounded::integer with a
// null_policy. However, if someone passes in something that is already a
// bounded::integer, this should not change the policy. Therefore, we default to
// something that is not a valid policy (void) and choose correct default
// behavior based on that. The policy must be the first parameter to work
// properly with automatic deduction of the type of `integer`, which means that
// the type of integer is not known when we are trying to determine the default
// policy.
//
// 2) The second is defaulted for the same reasons as the first.
//
// 3) The third parameter, T, has to be defaulted because we defaulted the first
// two. If T were the first template parameter, then we could not take advantage
// of function argument type deduction if the user specified either of the other
// two arguments. Therefore, we default it to something (it does not matter
// what), but the type will always be deduced as whatever we pass as the
// argument type.

template<typename overflow_policy = void, storage_type storage = storage_type::fast, typename T = void>
constexpr auto make(T const & value) noexcept -> equivalent_type<
	T,
	std::conditional_t<std::is_void<overflow_policy>::value,
		detail::equivalent_overflow_policy<T>,
		overflow_policy
	>,
	storage
> {
	static_assert(basic_numeric_limits<T>::is_specialized or std::is_enum<T>::value, "Argument to bounded::make must specialize std::numeric_limits or be an enum.");
	static_assert(basic_numeric_limits<T>::is_bounded or std::is_enum<T>::value, "Argument to bounded::make must be a bounded type.");
	return {value, non_check};
}

}	// namespace bounded
