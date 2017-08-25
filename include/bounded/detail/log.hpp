// Copyright David Stone 2015.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <bounded/detail/basic_numeric_limits.hpp>
#include <bounded/detail/class.hpp>
#include <bounded/detail/is_bounded_integer.hpp>
#include <bounded/detail/requires.hpp>

namespace bounded {
namespace detail {

template<typename Integer, typename Base>
constexpr auto log(Integer value, Base const base) noexcept {
	auto sum = 0;
	while (value >= base) {
		value /= base;
		++sum;
	}
	return sum;
}

}	// namespace detail

template<
	intmax_t minimum, intmax_t maximum, typename overflow_policy, storage_type storage, bool poisoned,
	typename Base,
	BOUNDED_REQUIRES(is_bounded_integer<Base>)
>
constexpr auto log(integer<minimum, maximum, overflow_policy, storage, poisoned> const value, Base const base) noexcept {
	static_assert(base > constant<1>, "Negative bases not currently supported.");
	static_assert(value > constant<0>, "The log of a negative number or zero is undefined.");
	using result_type = integer<
		detail::log(minimum, basic_numeric_limits<Base>::max()),
		detail::log(maximum, basic_numeric_limits<Base>::min()),
		overflow_policy,
		storage,
		poisoned
	>;
	using argument_type = integer<
		0,
		maximum,
		overflow_policy,
		storage,
		poisoned
	>;
	return result_type(detail::log(static_cast<argument_type>(value), base), non_check);
}

}	// namespace bounded

namespace std {
// This overload is required for std::sort to work on gcc with iterators whose
// difference_type is a bounded::integer. It is not guaranteed to continue to be
// supported. It returns a built-in integer that is the log2 of the number
template<intmax_t minimum, intmax_t maximum, typename overflow_policy, bounded::storage_type storage, bool poisoned>
constexpr auto __lg(bounded::integer<minimum, maximum, overflow_policy, storage, poisoned> const value) noexcept {
	return __lg(value.value());
}

}	// namespace std
