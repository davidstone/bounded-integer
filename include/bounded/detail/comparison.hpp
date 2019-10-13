// Copyright David Stone 2017.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <bounded/detail/is_bounded_integer.hpp>
#include <bounded/detail/max_builtin.hpp>
#include <bounded/detail/min_max_value.hpp>

#include <compare>
#include <cstddef>
#include <type_traits>

namespace bounded {
namespace detail {

template<typename LHS, typename RHS>
constexpr auto builtin_compare(LHS const lhs, RHS const rhs) {
	return
		lhs < rhs ? std::strong_ordering::less :
		lhs > rhs ? std::strong_ordering::greater :
		std::strong_ordering::equal;
}

} // namespace detail

constexpr auto compare(bool const lhs, bool const rhs) {
	return detail::builtin_compare(lhs, rhs);
}

template<typename LHS, typename RHS>
constexpr auto compare(LHS const * const lhs, RHS const * const rhs) {
	return detail::builtin_compare(lhs, rhs);
}

template<typename LHS, typename RHS> requires(std::is_floating_point_v<LHS> and std::is_floating_point_v<RHS>)
constexpr auto compare(LHS const lhs, RHS const rhs) {
	return detail::builtin_compare(lhs, rhs);
}

template<detail_builtin_integer LHS, detail_builtin_integer RHS>
constexpr auto compare(LHS const lhs, RHS const rhs) -> std::strong_ordering {
	if constexpr (detail_signed_builtin<LHS> == detail_signed_builtin<RHS>) {
		return detail::builtin_compare(lhs, rhs);
	} else if constexpr (not std::is_same_v<LHS, detail::max_unsigned_t> and not std::is_same_v<RHS, detail::max_unsigned_t>) {
		return detail::builtin_compare(static_cast<detail::max_signed_t>(lhs), static_cast<detail::max_signed_t>(rhs));
	} else if constexpr (detail_signed_builtin<LHS>) {
		static_assert(std::is_same_v<RHS, detail::max_unsigned_t>);
		return lhs < 0 ? std::strong_ordering::less : detail::builtin_compare(static_cast<RHS>(lhs), rhs);
	} else {
		static_assert(std::is_same_v<LHS, detail::max_unsigned_t>);
		return rhs < 0 ? std::strong_ordering::greater : detail::builtin_compare(lhs, static_cast<LHS>(rhs));
	}
}

template<typename Enum> requires(std::is_enum_v<Enum>)
constexpr auto compare(Enum const lhs, Enum const rhs) {
	using underlying = std::underlying_type_t<Enum>;
	return detail::builtin_compare(static_cast<underlying>(lhs), static_cast<underlying>(rhs));
}

namespace detail {

template<detail_builtin_integer LHS, detail_builtin_integer RHS>
constexpr auto safe_equal(LHS const lhs, RHS const rhs) -> bool {
	constexpr auto signed_max = max_value<detail::max_signed_t>;
	if constexpr (detail_signed_builtin<LHS> == detail_signed_builtin<RHS>) {
		return lhs == rhs;
	} else if constexpr (max_value<LHS> <= signed_max and max_value<RHS> <= signed_max) {
		return static_cast<detail::max_signed_t>(lhs) == static_cast<detail::max_signed_t>(rhs);
	} else if constexpr (detail_signed_builtin<LHS>) {
		static_assert(std::is_same_v<RHS, detail::max_unsigned_t>);
		return lhs >= 0 and static_cast<RHS>(lhs) == rhs;
	} else {
		static_assert(std::is_same_v<LHS, detail::max_unsigned_t>);
		return rhs >= 0 and static_cast<LHS>(rhs);
	}
}

template<typename Limited, typename UnaryFunction, typename LHS, typename RHS>
constexpr auto safe_extreme(UnaryFunction const pick_lhs, LHS const lhs_, RHS const rhs_) {
	auto normalized = [](auto const value) {
		using normalized_t = std::conditional_t<
			detail_signed_builtin<std::decay_t<decltype(value)>>,
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
constexpr auto safe_min(Ts... values) {
	return safe_extreme<max_signed_t>([](auto const cmp) { return cmp <= 0; }, values...);
}

template<typename... Ts>
constexpr auto safe_max(Ts... values) {
	return safe_extreme<max_unsigned_t>([](auto const cmp) { return cmp > 0; }, values...);
}

}	// namespace detail


template<bounded_integer LHS, bounded_integer RHS>
constexpr auto compare(LHS const & lhs, RHS const & rhs) {
	constexpr auto lhs_min = min_value<LHS>.value();
	constexpr auto lhs_max = max_value<LHS>.value();
	constexpr auto rhs_min = min_value<RHS>.value();
	constexpr auto rhs_max = max_value<RHS>.value();
	if constexpr (compare(lhs_min, rhs_max) > 0) {
		return std::strong_ordering::greater;
	} else if constexpr (compare(lhs_max, rhs_min) < 0) {
		return std::strong_ordering::less;
	} else if constexpr (compare(lhs_min, lhs_max) == 0 and compare(rhs_min, rhs_max) == 0 and compare(lhs_min, rhs_min) == 0) {
		return std::strong_ordering::equal;
	} else {
		return compare(lhs.value(), rhs.value());
	}
}


template<bounded_integer LHS, bounded_integer RHS>
constexpr auto operator==(LHS const & lhs, RHS const & rhs) {
	constexpr auto lhs_min = min_value<LHS>.value();
	constexpr auto lhs_max = max_value<LHS>.value();
	constexpr auto rhs_min = min_value<RHS>.value();
	constexpr auto rhs_max = max_value<RHS>.value();
	if constexpr (compare(lhs_min, rhs_max) > 0 or compare(lhs_max, rhs_min) < 0) {
		return false;
	} else if constexpr (compare(lhs_min, lhs_max) == 0 and compare(rhs_min, rhs_max) == 0 and compare(lhs_min, rhs_min) == 0) {
		return true;
	} else {
		return detail::safe_equal(lhs.value(), rhs.value());
	}
}

namespace detail {

template<typename LHS, typename RHS>
concept member_comparable = requires(LHS const & lhs, RHS const & rhs) { compare(lhs.compare(rhs), 0); };

} // namespace detail

template<typename LHS, typename RHS> requires detail::member_comparable<LHS, RHS>
constexpr auto compare(LHS const & lhs, RHS const & rhs) {
	return compare(lhs.compare(rhs), 0);
}

template<typename LHS, typename RHS> requires (detail::member_comparable<RHS, LHS> and !detail::member_comparable<LHS, RHS>)
constexpr auto compare(LHS const & lhs, RHS const & rhs) {
	return compare(0, rhs.compare(lhs));
}


template<typename LHS, typename RHS = LHS>
concept equality_comparable = requires(LHS const & lhs, RHS const & rhs) { lhs == rhs; };

template<typename LHS, typename RHS = LHS>
concept ordered = requires(LHS const & lhs, RHS const & rhs) { compare(lhs, rhs); };


template<typename LHS, typename RHS> requires equality_comparable<LHS, RHS>
constexpr auto operator!=(LHS const & lhs, RHS const & rhs) {
	return !(lhs == rhs);
}

template<typename LHS, typename RHS> requires ordered<LHS, RHS>
constexpr auto operator<(LHS const & lhs, RHS const & rhs) {
	return compare(lhs, rhs) < 0;
}
template<typename LHS, typename RHS> requires ordered<LHS, RHS>
constexpr auto operator>(LHS const & lhs, RHS const & rhs) {
	return compare(lhs, rhs) > 0;
}
template<typename LHS, typename RHS> requires ordered<LHS, RHS>
constexpr auto operator<=(LHS const & lhs, RHS const & rhs) {
	return compare(lhs, rhs) <= 0;
}
template<typename LHS, typename RHS> requires ordered<LHS, RHS>
constexpr auto operator>=(LHS const & lhs, RHS const & rhs) {
	return compare(lhs, rhs) >= 0;
}

#define BOUNDED_COMPARISON \
	using ::bounded::operator!=; \
	using ::bounded::operator<; \
	using ::bounded::operator>; \
	using ::bounded::operator<=; \
	using ::bounded::operator>=;

}	// namespace bounded
