// Copyright David Stone 2017.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <bounded/detail/class.hpp>
#include <bounded/detail/comparison.hpp>
#include <bounded/detail/common_type.hpp>

#include <cassert>
#include <limits>

namespace bounded {
namespace detail {

template<auto value_>
constexpr auto safer_negation(constant_t<value_> const value) noexcept {
	constexpr auto max_signed = std::numeric_limits<std::intmax_t>::max();
	if constexpr (value < constant<-max_signed>) {
		static_assert(value == constant<std::numeric_limits<std::intmax_t>::min()>);
		return -static_cast<std::uintmax_t>(value_);
	} else {
		static_assert(value <= constant<max_signed>, "Negation would overflow.");
		return -static_cast<std::intmax_t>(value_);
	}
}

template<typename T, BOUNDED_REQUIRES(std::is_signed<T>{})>
constexpr auto from_unsigned_cast(std::uintmax_t const value) noexcept {
	using limits = std::numeric_limits<std::intmax_t>;
	if (value <= limits::max()) {
		return static_cast<std::intmax_t>(value);
	} else {
		assert(value >= static_cast<std::uintmax_t>(limits::min()));
		return static_cast<std::intmax_t>(value - static_cast<std::uintmax_t>(limits::min())) + limits::min();
	}
}

template<typename T, BOUNDED_REQUIRES(std::is_unsigned<T>{})>
constexpr auto from_unsigned_cast(std::uintmax_t const value) noexcept {
	return value;
}

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
	return result_type(detail::from_unsigned_cast<typename result_type::underlying_type>(-static_cast<std::uintmax_t>(value)), non_check);
}

}	// namespace bounded

