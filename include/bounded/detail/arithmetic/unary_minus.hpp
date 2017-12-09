// Copyright David Stone 2017.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <bounded/detail/class.hpp>
#include <bounded/detail/comparison.hpp>
#include <bounded/detail/common_type.hpp>
#include <bounded/detail/max_builtin.hpp>

#include <cassert>
#include <limits>

namespace bounded {
namespace detail {

template<auto value_>
constexpr auto safer_negation(constant_t<value_> const value) noexcept {
	constexpr auto max_signed = basic_numeric_limits<max_signed_t>::max();
	if constexpr (value < constant<-max_signed>) {
		static_assert(value == constant<basic_numeric_limits<max_signed_t>::min()>);
		return -static_cast<max_unsigned_t>(value_);
	} else {
		static_assert(value <= constant<max_signed>, "Negation would overflow.");
		return -static_cast<max_signed_t>(value_);
	}
}

template<typename T, BOUNDED_REQUIRES(is_signed_builtin<T>)>
constexpr auto from_unsigned_cast(max_unsigned_t const value) noexcept {
	using limits = basic_numeric_limits<max_signed_t>;
	if (value <= limits::max()) {
		return static_cast<max_signed_t>(value);
	} else {
		assert(value >= static_cast<max_unsigned_t>(limits::min()));
		return static_cast<max_signed_t>(value - static_cast<max_unsigned_t>(limits::min())) + limits::min();
	}
}

template<typename T, BOUNDED_REQUIRES(is_unsigned_builtin<T>)>
constexpr auto from_unsigned_cast(max_unsigned_t const value) noexcept {
	return value;
}

template<typename T>
struct promoted_unsigned_c {
	using type = std::conditional_t<sizeof(T) <= sizeof(unsigned), unsigned, std::make_unsigned_t<T>>;
};

template<>
struct promoted_unsigned_c<max_signed_t> {
	using type = max_unsigned_t;
};
template<>
struct promoted_unsigned_c<max_unsigned_t> {
	using type = max_unsigned_t;
};

template<typename T>
using promoted_unsigned = typename promoted_unsigned_c<T>::type;

}	// namespace detail

template<auto minimum, auto maximum, typename overflow_policy, storage_type storage, bool poisoned>
constexpr auto operator-(integer<minimum, maximum, overflow_policy, storage, poisoned> const value) noexcept {
	static_assert(
		(constant<minimum> == constant<maximum>) ==
		(detail::safer_negation(constant<minimum>) == detail::safer_negation(constant<maximum>))
	);
	using result_type = integer<
		detail::safer_negation(constant<maximum>),
		detail::safer_negation(constant<minimum>),
		overflow_policy,
		storage,
		poisoned
	>;
	using underlying_type = typename result_type::underlying_type;
	using unsigned_type = detail::promoted_unsigned<underlying_type>;
	return result_type(detail::from_unsigned_cast<typename result_type::underlying_type>(-static_cast<unsigned_type>(value)), non_check);
}

}	// namespace bounded

