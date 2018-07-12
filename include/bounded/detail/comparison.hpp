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
#include <bounded/detail/strong_ordering.hpp>

#include <functional>
#include <type_traits>

namespace bounded {

template<typename LHS, typename RHS>
constexpr auto compare(LHS * const lhs, RHS * const rhs) noexcept {
	return lhs == rhs ? strong_ordering_equal : std::less{}(lhs, rhs) ? strong_ordering_less : strong_ordering_greater;
}

// TODO: Implement correctly
template<typename LHS, typename RHS, BOUNDED_REQUIRES(std::is_floating_point_v<LHS> and std::is_floating_point_v<RHS>)>
constexpr auto compare(LHS const lhs, RHS const rhs) noexcept {
	return lhs == rhs ? strong_ordering_equal : lhs < rhs ? strong_ordering_less : strong_ordering_greater;
}

template<typename LHS, typename RHS, BOUNDED_REQUIRES(detail::is_builtin_integer<LHS> and detail::is_builtin_integer<RHS>)>
constexpr auto compare(LHS const lhs, RHS const rhs) noexcept -> strong_ordering const {
	if constexpr (detail::is_signed_builtin<LHS> == detail::is_signed_builtin<RHS>) {
		return
			(lhs < rhs) ? strong_ordering_less :
			(lhs > rhs) ? strong_ordering_greater :
			strong_ordering_equal;
	} else if constexpr (not std::is_same_v<LHS, detail::max_unsigned_t> and not std::is_same_v<RHS, detail::max_unsigned_t>) {
		return compare(static_cast<detail::max_signed_t>(lhs), static_cast<detail::max_signed_t>(rhs));
	} else if constexpr (detail::is_signed_builtin<LHS>) {
		static_assert(std::is_same_v<RHS, detail::max_unsigned_t>);
		return (lhs < static_cast<LHS>(basic_numeric_limits<RHS>::min()) or basic_numeric_limits<LHS>::max() < rhs) ?
			strong_ordering_less :
			compare(lhs, static_cast<LHS>(rhs));
	} else {
		static_assert(std::is_same_v<LHS, detail::max_unsigned_t>);
		return (rhs < static_cast<RHS>(basic_numeric_limits<LHS>::min()) or basic_numeric_limits<RHS>::max() < lhs) ?
			strong_ordering_greater :
			compare(static_cast<RHS>(lhs), rhs);
	}
}

template<typename Enum, BOUNDED_REQUIRES(std::is_enum_v<Enum>)>
constexpr auto compare(Enum const lhs, Enum const rhs) noexcept {
	using underlying = std::underlying_type_t<Enum>;
	return bounded::compare(static_cast<underlying>(lhs), static_cast<underlying>(rhs));
}

namespace detail {

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

// Variadic makes this function always a worse match than the above
template<typename LHS, typename RHS, typename... Ignore, BOUNDED_REQUIRES(sizeof...(Ignore) == 0)>
constexpr auto compare(LHS const & lhs, RHS const & rhs, Ignore...) BOUNDED_NOEXCEPT_DECLTYPE(
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
