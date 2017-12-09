// Copyright David Stone 2017.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <bounded/detail/basic_numeric_limits.hpp>
#include <bounded/detail/is_bounded_integer.hpp>
#include <bounded/detail/max_builtin.hpp>
#include <bounded/detail/noexcept.hpp>
#include <bounded/detail/requires.hpp>

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


template<typename LHS, typename RHS, BOUNDED_REQUIRES(detail::is_builtin_integer<LHS> and detail::is_builtin_integer<RHS>)>
constexpr auto compare(LHS const lhs, RHS const rhs) noexcept -> strong_ordering const {
	if constexpr (detail::is_signed_builtin<LHS> == detail::is_signed_builtin<RHS>) {
		return
			(lhs < rhs) ? strong_ordering_less :
			(lhs > rhs) ? strong_ordering_greater :
			strong_ordering_equal;
	} else if constexpr (not std::is_same<LHS, detail::max_unsigned_t>{} and not std::is_same<RHS, detail::max_unsigned_t>{}) {
		return compare(static_cast<detail::max_signed_t>(lhs), static_cast<detail::max_signed_t>(rhs));
	} else if constexpr (detail::is_signed_builtin<LHS>) {
		static_assert(std::is_same<RHS, detail::max_unsigned_t>{});
		return (lhs < static_cast<LHS>(basic_numeric_limits<RHS>::min()) or basic_numeric_limits<LHS>::max() < rhs) ?
			strong_ordering_less :
			compare(lhs, static_cast<LHS>(rhs));
	} else {
		static_assert(std::is_same<LHS, detail::max_unsigned_t>{});
		return (rhs < static_cast<RHS>(basic_numeric_limits<LHS>::min()) or basic_numeric_limits<RHS>::max() < lhs) ?
			strong_ordering_greater :
			compare(static_cast<RHS>(lhs), rhs);
	}
}


namespace detail {

template<typename Limited, typename UnaryFunction, typename LHS, typename RHS>
constexpr auto safe_extreme(LHS const lhs, RHS const rhs, UnaryFunction const pick_lhs) noexcept {
	static_assert(std::is_same<LHS, max_signed_t>{} or std::is_same<LHS, max_unsigned_t>{});
	static_assert(std::is_same<RHS, max_signed_t>{} or std::is_same<RHS, max_unsigned_t>{});
	using result_type = std::conditional_t<std::is_same<LHS, RHS>{}, LHS, Limited>;
	return (pick_lhs(compare(lhs, rhs))) ?
		static_cast<result_type>(lhs) :
		static_cast<result_type>(rhs);
}

template<typename LHS, typename RHS>
constexpr auto safe_min(LHS const lhs, RHS const rhs) noexcept {
	return safe_extreme<max_signed_t>(lhs, rhs, [](auto const cmp) { return cmp <= 0; });
}

template<typename LHS, typename RHS>
constexpr auto safe_max(LHS const lhs, RHS const rhs) noexcept {
	return safe_extreme<max_unsigned_t>(lhs, rhs, [](auto const cmp) { return cmp > 0; });
}

}	// namespace detail


template<typename LHS, typename RHS, BOUNDED_REQUIRES(is_bounded_integer<LHS> and is_bounded_integer<RHS>)>
constexpr auto compare(LHS const & lhs, RHS const & rhs) noexcept {
	using lhs_limits = basic_numeric_limits<LHS>;
	using rhs_limits = basic_numeric_limits<RHS>;
	if constexpr (compare(lhs_limits::min(), rhs_limits::max()) > 0) {
		static_cast<void>(lhs);
		static_cast<void>(rhs);
		return strong_ordering_greater;
	} else if constexpr (compare(lhs_limits::max(), rhs_limits::min()) < 0) {
		static_cast<void>(lhs);
		static_cast<void>(rhs);
		return strong_ordering_less;
	} else if constexpr (compare(lhs_limits::min(), lhs_limits::max()) == 0 and compare(rhs_limits::min(), rhs_limits::max()) == 0 and compare(lhs_limits::min(), rhs_limits::min()) == 0) {
		static_cast<void>(lhs);
		static_cast<void>(rhs);
		return strong_ordering_equal;
	} else {
		return compare(lhs.value(), rhs.value());
	}
}


template<typename LHS, typename RHS>
constexpr auto compare(LHS const & lhs, RHS const & rhs) BOUNDED_NOEXCEPT_DECLTYPE(
	strong_ordering(lhs.compare(rhs))
)

// C-style variadic makes this function always a worse match than the above
template<typename LHS, typename RHS>
constexpr auto compare(LHS const & lhs, RHS const & rhs, ...) BOUNDED_NOEXCEPT_DECLTYPE(
	strong_ordering(-rhs.compare(lhs))
)


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
