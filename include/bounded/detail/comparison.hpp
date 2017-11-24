// Copyright David Stone 2017.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <bounded/detail/basic_numeric_limits.hpp>
#include <bounded/detail/is_bounded_integer.hpp>
#include <bounded/detail/noexcept.hpp>
#include <bounded/detail/requires.hpp>
#include <bounded/detail/underlying_type.hpp>

#include <cstdint>
#include <type_traits>

namespace bounded {

struct strong_ordering {
	constexpr explicit strong_ordering(int value_) noexcept:
		value(value_)
	{
	}

	friend constexpr auto operator==(strong_ordering const lhs, std::nullptr_t) noexcept {
		return lhs.value == 0;
	}
	friend constexpr auto operator==(std::nullptr_t, strong_ordering const rhs) noexcept {
		return 0 == rhs.value;
	}

	friend constexpr auto operator!=(strong_ordering const lhs, std::nullptr_t) noexcept {
		return lhs.value != 0;
	}
	friend constexpr auto operator!=(std::nullptr_t, strong_ordering const rhs) noexcept {
		return 0 != rhs.value;
	}

	friend constexpr auto operator<(strong_ordering const lhs, std::nullptr_t) noexcept {
		return lhs.value < 0;
	}
	friend constexpr auto operator<(std::nullptr_t, strong_ordering const rhs) noexcept {
		return 0 < rhs.value;
	}

	friend constexpr auto operator>(strong_ordering const lhs, std::nullptr_t) noexcept {
		return lhs.value > 0;
	}
	friend constexpr auto operator>(std::nullptr_t, strong_ordering const rhs) noexcept {
		return 0 > rhs.value;
	}

	friend constexpr auto operator<=(strong_ordering const lhs, std::nullptr_t) noexcept {
		return lhs.value <= 0;
	}
	friend constexpr auto operator<=(std::nullptr_t, strong_ordering const rhs) noexcept {
		return 0 <= rhs.value;
	}

	friend constexpr auto operator>=(strong_ordering const lhs, std::nullptr_t) noexcept {
		return lhs.value >= 0;
	}
	friend constexpr auto operator>=(std::nullptr_t, strong_ordering const rhs) noexcept {
		return 0 >= rhs.value;
	}

private:
	int value;
};

constexpr auto strong_ordering_less = strong_ordering(-1);
constexpr auto strong_ordering_equal = strong_ordering(0);
constexpr auto strong_ordering_greater = strong_ordering(1);


namespace detail {

template<typename T>
constexpr auto is_max_unsigned =
	std::is_unsigned<T>{} and
	static_cast<std::uintmax_t>(std::numeric_limits<T>::min()) == std::numeric_limits<std::uintmax_t>::min() and
	static_cast<std::uintmax_t>(std::numeric_limits<T>::max()) == std::numeric_limits<std::uintmax_t>::max();

}	// namespace detail

template<typename LHS, typename RHS, BOUNDED_REQUIRES(std::is_integral<LHS>{} and std::is_integral<RHS>{})>
constexpr auto compare(LHS const lhs, RHS const rhs) noexcept -> strong_ordering const {
	if constexpr (std::is_signed<LHS>{} == std::is_signed<RHS>{}) {
		return
			(lhs < rhs) ? strong_ordering_less :
			(lhs > rhs) ? strong_ordering_greater :
			strong_ordering_equal;
	} else if constexpr (not detail::is_max_unsigned<LHS> and not detail::is_max_unsigned<RHS>) {
		return compare(static_cast<std::intmax_t>(lhs), static_cast<std::intmax_t>(rhs));
	} else if constexpr (std::is_signed<LHS>{}) {
		static_assert(detail::is_max_unsigned<RHS>);
		return (lhs < static_cast<LHS>(std::numeric_limits<RHS>::min()) or std::numeric_limits<LHS>::max() < rhs) ?
			strong_ordering_less :
			compare(lhs, static_cast<LHS>(rhs));
	} else {
		static_assert(detail::is_max_unsigned<LHS>);
		return (rhs < static_cast<RHS>(std::numeric_limits<LHS>::min()) or std::numeric_limits<RHS>::max() < lhs) ?
			strong_ordering_greater :
			compare(static_cast<RHS>(lhs), rhs);
	}
}


namespace detail {

template<typename Limited, typename UnaryFunction, typename LHS, typename RHS>
constexpr auto safe_extreme(LHS const lhs, RHS const rhs, UnaryFunction const pick_lhs) noexcept {
	static_assert(std::is_same<LHS, std::intmax_t>{} or std::is_same<LHS, std::uintmax_t>{});
	static_assert(std::is_same<RHS, std::intmax_t>{} or std::is_same<RHS, std::uintmax_t>{});
	using result_type = std::conditional_t<std::is_same<LHS, RHS>{}, LHS, Limited>;
	return (pick_lhs(compare(lhs, rhs))) ?
		static_cast<result_type>(lhs) :
		static_cast<result_type>(rhs);
}

template<typename LHS, typename RHS>
constexpr auto safe_min(LHS const lhs, RHS const rhs) noexcept {
	return safe_extreme<std::intmax_t>(lhs, rhs, [](auto const cmp) { return cmp <= 0; });
}

template<typename LHS, typename RHS>
constexpr auto safe_max(LHS const lhs, RHS const rhs) noexcept {
	return safe_extreme<std::uintmax_t>(lhs, rhs, [](auto const cmp) { return cmp > 0; });
}

}	// namespace detail


template<typename LHS, typename RHS, BOUNDED_REQUIRES(is_bounded_integer<LHS> and is_bounded_integer<RHS>)>
constexpr auto compare(LHS const & lhs, RHS const & rhs) noexcept {
	using lhs_limits = basic_numeric_limits<LHS>;
	using rhs_limits = basic_numeric_limits<RHS>;
	if constexpr (lhs_limits::min() > rhs_limits::max()) {
		static_cast<void>(lhs);
		static_cast<void>(rhs);
		return strong_ordering_greater;
	} else if constexpr (lhs_limits::max() < rhs_limits::min()) {
		static_cast<void>(lhs);
		static_cast<void>(rhs);
		return strong_ordering_less;
	} else if constexpr (lhs_limits::min() == lhs_limits::max() and rhs_limits::min() == rhs_limits::max() and lhs_limits::min() == rhs_limits::min()) {
		static_cast<void>(lhs);
		static_cast<void>(rhs);
		return strong_ordering_equal;
	} else {
		return compare(lhs.value(), rhs.value());
	}
}


template<typename LHS, typename RHS>
constexpr auto operator==(LHS const & lhs, RHS const & rhs) BOUNDED_NOEXCEPT_DECLTYPE(
	compare(lhs, rhs) == 0
)
template<typename LHS, typename RHS>
constexpr auto operator!=(LHS const & lhs, RHS const & rhs) BOUNDED_NOEXCEPT_DECLTYPE(
	compare(lhs, rhs) != 0
)

template<typename LHS, typename RHS>
constexpr auto operator<(LHS const & lhs, RHS const & rhs) BOUNDED_NOEXCEPT_DECLTYPE(
	compare(lhs, rhs) < 0
)
template<typename LHS, typename RHS>
constexpr auto operator>(LHS const & lhs, RHS const & rhs) BOUNDED_NOEXCEPT_DECLTYPE(
	compare(lhs, rhs) > 0
)
template<typename LHS, typename RHS>
constexpr auto operator<=(LHS const & lhs, RHS const & rhs) BOUNDED_NOEXCEPT_DECLTYPE(
	compare(lhs, rhs) <= 0
)
template<typename LHS, typename RHS>
constexpr auto operator>=(LHS const & lhs, RHS const & rhs) BOUNDED_NOEXCEPT_DECLTYPE(
	compare(lhs, rhs) >= 0
)

#define BOUNDED_COMPARISON \
	using ::bounded::operator==; \
	using ::bounded::operator!=; \
	using ::bounded::operator<; \
	using ::bounded::operator>; \
	using ::bounded::operator<=; \
	using ::bounded::operator>=;
	
}	// namespace bounded