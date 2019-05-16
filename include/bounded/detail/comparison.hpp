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

#include <functional>
#include <type_traits>

namespace bounded {

struct strong_ordering {
	static strong_ordering const equal;
	static strong_ordering const greater;
	static strong_ordering const less;
	
	friend constexpr auto operator==(strong_ordering const lhs, std::nullptr_t) noexcept {
		return lhs.value == 0;
	}
	friend constexpr auto operator!=(strong_ordering const lhs, std::nullptr_t) noexcept {
		return lhs.value != 0;
	}
	friend constexpr auto operator<(strong_ordering const lhs, std::nullptr_t) noexcept {
		return lhs.value < 0;
	}
	friend constexpr auto operator>(strong_ordering const lhs, std::nullptr_t) noexcept {
		return lhs.value > 0;
	}
	friend constexpr auto operator<=(strong_ordering const lhs, std::nullptr_t) noexcept {
		return lhs.value <= 0;
	}
	friend constexpr auto operator>=(strong_ordering const lhs, std::nullptr_t) noexcept {
		return lhs.value >= 0;
	}
	
	friend constexpr auto operator==(std::nullptr_t, strong_ordering const rhs) noexcept {
		return 0 == rhs.value;
	}
	friend constexpr auto operator!=(std::nullptr_t, strong_ordering const rhs) noexcept {
		return 0 != rhs.value;
	}
	friend constexpr auto operator<(std::nullptr_t, strong_ordering const rhs) noexcept {
		return 0 < rhs.value;
	}
	friend constexpr auto operator>(std::nullptr_t, strong_ordering const rhs) noexcept {
		return 0 > rhs.value;
	}
	friend constexpr auto operator<=(std::nullptr_t, strong_ordering const rhs) noexcept {
		return 0 <= rhs.value;
	}
	friend constexpr auto operator>=(std::nullptr_t, strong_ordering const rhs) noexcept {
		return 0 >= rhs.value;
	}
	
private:
	constexpr explicit strong_ordering(int value_):
		value(value_)
	{
	}

	int value;
};

constexpr inline strong_ordering strong_ordering::equal(0);
constexpr inline strong_ordering strong_ordering::greater(1);
constexpr inline strong_ordering strong_ordering::less(-1);

template<typename LHS, typename RHS>
constexpr auto compare(LHS const & lhs, RHS const & rhs) BOUNDED_NOEXCEPT_DECLTYPE(
	compare(lhs.compare(rhs), 0)
)

// Variadic makes this function always a worse match than the above
template<typename LHS, typename RHS, typename... Ignore, BOUNDED_REQUIRES(sizeof...(Ignore) == 0)>
constexpr auto compare(LHS const & lhs, RHS const & rhs, Ignore...) BOUNDED_NOEXCEPT_DECLTYPE(
	compare(0, rhs.compare(lhs))
)

#if 0
template<typename LHS, typename RHS, BOUNDED_REQUIRES(not detail::is_builtin_integer<LHS> or !detail::is_builtin_integer<RHS>)>
constexpr auto compare(LHS const & lhs, RHS const & rhs) BOUNDED_NOEXCEPT_DECLTYPE(
	lhs <=> rhs
)
#endif

namespace detail {

template<typename LHS, typename RHS>
constexpr auto builtin_compare(LHS const lhs, RHS const rhs) noexcept {
	return
		lhs < rhs ? strong_ordering::less :
		lhs > rhs ? strong_ordering::greater :
		strong_ordering::equal;
}

} // namespace detail

template<typename LHS, typename RHS>
constexpr auto compare(LHS const * const lhs, RHS const * const rhs) BOUNDED_NOEXCEPT_DECLTYPE(
	detail::builtin_compare(lhs, rhs)
)

template<typename LHS, typename RHS, BOUNDED_REQUIRES(std::is_floating_point_v<LHS> and std::is_floating_point_v<RHS>)>
constexpr auto compare(LHS const lhs, RHS const rhs) BOUNDED_NOEXCEPT_DECLTYPE(
	detail::builtin_compare(lhs, rhs)
)

template<typename LHS, typename RHS, BOUNDED_REQUIRES(detail::is_builtin_integer<LHS> and detail::is_builtin_integer<RHS>)>
constexpr auto compare(LHS const lhs, RHS const rhs) noexcept -> strong_ordering {
	if constexpr (detail::is_signed_builtin<LHS> == detail::is_signed_builtin<RHS>) {
		return detail::builtin_compare(lhs, rhs);
	} else if constexpr (not std::is_same_v<LHS, detail::max_unsigned_t> and not std::is_same_v<RHS, detail::max_unsigned_t>) {
		return detail::builtin_compare(static_cast<detail::max_signed_t>(lhs), static_cast<detail::max_signed_t>(rhs));
	} else if constexpr (detail::is_signed_builtin<LHS>) {
		static_assert(std::is_same_v<RHS, detail::max_unsigned_t>);
		return lhs < 0 ? strong_ordering::less : detail::builtin_compare(static_cast<RHS>(lhs), rhs);
	} else {
		static_assert(std::is_same_v<LHS, detail::max_unsigned_t>);
		return rhs < 0 ? strong_ordering::greater : detail::builtin_compare(lhs, static_cast<LHS>(rhs));
	}
}

namespace detail {

template<typename LHS, typename RHS, BOUNDED_REQUIRES(detail::is_builtin_integer<LHS> and detail::is_builtin_integer<RHS>)>
constexpr auto safe_equal(LHS const lhs, RHS const rhs) noexcept -> bool {
	constexpr auto signed_max [[maybe_unused]] = basic_numeric_limits<detail::max_signed_t>::max();
	if constexpr (detail::is_signed_builtin<LHS> == detail::is_signed_builtin<RHS>) {
		return lhs == rhs;
	} else if constexpr (basic_numeric_limits<LHS>::max() <= signed_max and basic_numeric_limits<RHS>::max() <= signed_max) {
		return static_cast<detail::max_signed_t>(lhs) == static_cast<detail::max_signed_t>(rhs);
	} else if constexpr (detail::is_signed_builtin<LHS>) {
		static_assert(std::is_same_v<RHS, detail::max_unsigned_t>);
		return lhs >= 0 and static_cast<RHS>(lhs) == rhs;
	} else {
		static_assert(std::is_same_v<LHS, detail::max_unsigned_t>);
		return rhs >= 0 and static_cast<LHS>(rhs);
	}
}

template<typename Limited, typename UnaryFunction, typename LHS, typename RHS>
constexpr auto safe_extreme(UnaryFunction const pick_lhs, LHS const lhs_, RHS const rhs_) noexcept {
	auto normalized = [](auto const value) {
		using normalized_t = std::conditional_t<
			is_signed_builtin<std::decay_t<decltype(value)>>,
			max_signed_t,
			max_unsigned_t
		>;
		return static_cast<normalized_t>(value);
	};
	auto const lhs = normalized(lhs_);
	auto const rhs = normalized(rhs_);
	using result_type = std::conditional_t<std::is_same_v<decltype(lhs), decltype(rhs)>, decltype(lhs), Limited>;
	return (pick_lhs(compare(lhs, rhs))) ?
		static_cast<result_type>(lhs) :
		static_cast<result_type>(rhs);
}

template<typename Limited, typename UnaryFunction, typename LHS, typename RHS, typename... Rest>
constexpr auto safe_extreme(UnaryFunction const pick_lhs, LHS const lhs, RHS const rhs, Rest const ... rest) {
	return safe_extreme<Limited>(pick_lhs, safe_extreme<Limited>(pick_lhs, lhs, rhs), rest...);
}

template<typename... Ts>
constexpr auto safe_min(Ts... values) noexcept {
	return safe_extreme<max_signed_t>([](auto const cmp) { return cmp <= 0; }, values...);
}

template<typename... Ts>
constexpr auto safe_max(Ts... values) noexcept {
	return safe_extreme<max_unsigned_t>([](auto const cmp) { return cmp > 0; }, values...);
}

}	// namespace detail


template<typename LHS, typename RHS, BOUNDED_REQUIRES(is_bounded_integer<LHS> and is_bounded_integer<RHS>)>
constexpr auto compare(LHS const & lhs [[maybe_unused]], RHS const & rhs [[maybe_unused]]) noexcept {
	using lhs_limits = basic_numeric_limits<LHS>;
	using rhs_limits = basic_numeric_limits<RHS>;
	if constexpr (compare(lhs_limits::min(), rhs_limits::max()) > 0) {
		return strong_ordering::greater;
	} else if constexpr (compare(lhs_limits::max(), rhs_limits::min()) < 0) {
		return strong_ordering::less;
	} else if constexpr (compare(lhs_limits::min(), lhs_limits::max()) == 0 and compare(rhs_limits::min(), rhs_limits::max()) == 0 and compare(lhs_limits::min(), rhs_limits::min()) == 0) {
		return strong_ordering::equal;
	} else {
		return compare(lhs.value(), rhs.value());
	}
}


template<typename LHS, typename RHS, BOUNDED_REQUIRES(is_bounded_integer<LHS> and is_bounded_integer<RHS>)>
constexpr auto operator==(LHS const & lhs [[maybe_unused]], RHS const & rhs [[maybe_unused]]) noexcept {
	using lhs_limits = basic_numeric_limits<LHS>;
	using rhs_limits = basic_numeric_limits<RHS>;
	if constexpr (compare(lhs_limits::min(), rhs_limits::max()) > 0 or compare(lhs_limits::max(), rhs_limits::min()) < 0) {
		return false;
	} else if constexpr (compare(lhs_limits::min(), lhs_limits::max()) == 0 and compare(rhs_limits::min(), rhs_limits::max()) == 0 and compare(lhs_limits::min(), rhs_limits::min()) == 0) {
		return true;
	} else {
		return detail::safe_equal(lhs.value(), rhs.value());
	}
}



template<typename LHS, typename RHS>
constexpr auto operator!=(LHS const & lhs, RHS const & rhs) BOUNDED_NOEXCEPT_DECLTYPE(
	!(lhs == rhs)
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
	using ::bounded::operator!=; \
	using ::bounded::operator<; \
	using ::bounded::operator>; \
	using ::bounded::operator<=; \
	using ::bounded::operator>=;


}	// namespace bounded
