// Copyright David Stone 2017.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <bounded/detail/builtin_min_max_value.hpp>
#include <bounded/detail/is_bounded_integer.hpp>
#include <bounded/detail/max_builtin.hpp>
#include <bounded/detail/min_max_value.hpp>

#include <compare>
#include <type_traits>

namespace bounded {
namespace detail {

template<builtin_integer LHS, builtin_integer RHS>
constexpr auto safe_compare(LHS const lhs, RHS const rhs) -> std::strong_ordering {
	if constexpr (signed_builtin<LHS> == signed_builtin<RHS>) {
		return lhs <=> rhs;
	} else if constexpr (not std::is_same_v<LHS, detail::max_unsigned_t> and not std::is_same_v<RHS, detail::max_unsigned_t>) {
		return static_cast<detail::max_signed_t>(lhs) <=> static_cast<detail::max_signed_t>(rhs);
	} else if constexpr (signed_builtin<LHS>) {
		static_assert(std::is_same_v<RHS, detail::max_unsigned_t>);
		return lhs < 0 ? std::strong_ordering::less : static_cast<RHS>(lhs) <=> rhs;
	} else {
		static_assert(std::is_same_v<LHS, detail::max_unsigned_t>);
		return rhs < 0 ? std::strong_ordering::greater : lhs <=> static_cast<LHS>(rhs);
	}
}

template<builtin_integer LHS, builtin_integer RHS>
constexpr auto safe_equal(LHS const lhs, RHS const rhs) -> bool {
	constexpr auto signed_max = max_value<detail::max_signed_t>;
	if constexpr (signed_builtin<LHS> == signed_builtin<RHS>) {
		return lhs == rhs;
	} else if constexpr (max_value<LHS> <= signed_max and max_value<RHS> <= signed_max) {
		return static_cast<detail::max_signed_t>(lhs) == static_cast<detail::max_signed_t>(rhs);
	} else if constexpr (signed_builtin<LHS>) {
		static_assert(std::is_same_v<RHS, detail::max_unsigned_t>);
		return lhs >= 0 and static_cast<RHS>(lhs) == rhs;
	} else {
		static_assert(std::is_same_v<LHS, detail::max_unsigned_t>);
		return rhs >= 0 and lhs == static_cast<LHS>(rhs);
	}
}

template<typename Limited>
constexpr auto safe_extreme(auto const pick_lhs, auto const lhs_, auto const rhs_) {
	auto normalized = [](auto const value) {
		using normalized_t = std::conditional_t<
			signed_builtin<std::decay_t<decltype(value)>>,
			max_signed_t,
			max_unsigned_t
		>;
		return static_cast<normalized_t>(value);
	};
	auto const lhs = normalized(lhs_);
	auto const rhs = normalized(rhs_);
	using result_type = std::conditional_t<std::is_same_v<decltype(lhs), decltype(rhs)>, decltype(lhs), Limited>;
	return (pick_lhs(safe_compare(lhs, rhs))) ?
		static_cast<result_type>(lhs) :
		static_cast<result_type>(rhs);
}

template<typename Limited>
constexpr auto safe_extreme(auto const pick_lhs, auto const lhs, auto const rhs, auto const ... rest) {
	return safe_extreme<Limited>(pick_lhs, safe_extreme<Limited>(pick_lhs, lhs, rhs), rest...);
}

constexpr auto safe_min(auto... values) {
	return safe_extreme<max_signed_t>([](auto const cmp) { return cmp <= 0; }, values...);
}

constexpr auto safe_max(auto... values) {
	return safe_extreme<max_unsigned_t>([](auto const cmp) { return cmp > 0; }, values...);
}

}	// namespace detail


template<bounded_integer LHS, bounded_integer RHS>
constexpr auto operator<=>(LHS const & lhs, RHS const & rhs) -> std::strong_ordering {
	constexpr auto lhs_min = detail::builtin_min_value<LHS>;
	constexpr auto lhs_max = detail::builtin_max_value<LHS>;
	constexpr auto rhs_min = detail::builtin_min_value<RHS>;
	constexpr auto rhs_max = detail::builtin_max_value<RHS>;
	if constexpr (detail::safe_compare(lhs_min, rhs_max) > 0) {
		return std::strong_ordering::greater;
	} else if constexpr (detail::safe_compare(lhs_max, rhs_min) < 0) {
		return std::strong_ordering::less;
	} else if constexpr (detail::safe_equal(lhs_min, lhs_max) and detail::safe_equal(rhs_min, rhs_max) and detail::safe_equal(lhs_min, rhs_min)) {
		return std::strong_ordering::equal;
	} else {
		return detail::safe_compare(lhs.value(), rhs.value());
	}
}

template<bounded_integer LHS, bounded_integer RHS>
constexpr auto operator==(LHS const & lhs, RHS const & rhs) -> bool {
	constexpr auto lhs_min = detail::builtin_min_value<LHS>;
	constexpr auto lhs_max = detail::builtin_max_value<LHS>;
	constexpr auto rhs_min = detail::builtin_min_value<RHS>;
	constexpr auto rhs_max = detail::builtin_max_value<RHS>;
	if constexpr (detail::safe_compare(lhs_min, rhs_max) > 0 or detail::safe_compare(lhs_max, rhs_min) < 0) {
		return false;
	} else if constexpr (detail::safe_equal(lhs_min, lhs_max) and detail::safe_equal(rhs_min, rhs_max) and detail::safe_equal(lhs_min, rhs_min)) {
		return true;
	} else {
		return detail::safe_equal(lhs.value(), rhs.value());
	}
}

namespace detail {

template<typename LHS, typename RHS>
concept member_comparable = requires(LHS const & lhs, RHS const & rhs) { lhs.compare(rhs) <=> 0; };

} // namespace detail

template<typename LHS, typename RHS> requires detail::member_comparable<LHS, RHS>
constexpr auto operator<=>(LHS const & lhs, RHS const & rhs) {
	return lhs.compare(rhs) <=> 0;
}


template<typename LHS, typename RHS = LHS>
concept equality_comparable = requires(LHS const & lhs, RHS const & rhs) { lhs == rhs; };

template<typename LHS, typename RHS = LHS>
concept ordered = requires(LHS const & lhs, RHS const & rhs) { lhs <=> rhs; };


}	// namespace bounded
