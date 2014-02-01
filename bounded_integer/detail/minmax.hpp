// Compile-time min and max of an arbitrary number of values
// Copyright (C) 2014 David Stone
//
// This program is free software: you can redistribute it and / or modify
// it under the terms of the GNU Affero General Public License as
// published by the Free Software Foundation, either version 3 of the
// License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU Affero General Public License for more details.
//
// You should have received a copy of the GNU Affero General Public License
// along with this program. If not, see <http://www.gnu.org/licenses/>.

#ifndef BOUNDED_INTEGER_MINMAX_HPP_
#define BOUNDED_INTEGER_MINMAX_HPP_

#include "common_policy.hpp"
#include "forward_declaration.hpp"
#include "make_bounded.hpp"
#include "numeric_limits.hpp"
#include "overlapping_range.hpp"

namespace bounded_integer {
namespace detail {
namespace minmax {

class less {
public:
	template<typename LHS, typename RHS>
	constexpr bool operator()(LHS && lhs, RHS && rhs) const noexcept {
		static_assert(noexcept(std::forward<LHS>(lhs) < std::forward<RHS>(rhs)), "< must be noexcept.");
		return std::forward<LHS>(lhs) < std::forward<RHS>(rhs);
	}
};
class greater {
public:
	template<typename LHS, typename RHS>
	constexpr bool operator()(LHS && lhs, RHS && rhs) const noexcept {
		static_assert(noexcept(std::forward<LHS>(lhs) > std::forward<RHS>(rhs)), "> must be noexcept.");
		return std::forward<LHS>(lhs) > std::forward<RHS>(rhs);
	}
};

template<typename compare, typename... Ts>
class minmax_type;

template<typename compare, typename... Ts>
using minmax_t = typename minmax_type<compare, Ts...>::type;

template<typename compare, typename T0, typename T1, typename... Ts>
class minmax_type<compare, T0, T1, Ts...> {
public:
	using type = minmax_t<compare, T0, minmax_t<compare, T1, Ts...>>;
};

template<
	typename compare,
	intmax_t lhs_min, intmax_t lhs_max, typename lhs_policy, bounds lhs_bound,
	intmax_t rhs_min, intmax_t rhs_max, typename rhs_policy, bounds rhs_bound
>
class minmax_type<compare, bounded_integer<lhs_min, lhs_max, lhs_policy, lhs_bound>, bounded_integer<rhs_min, rhs_max, rhs_policy, rhs_bound>> {
private:
	static constexpr intmax_t minimum = compare{}(lhs_min, rhs_min) ? lhs_min : rhs_min;
	static constexpr intmax_t maximum = compare{}(lhs_max, rhs_max) ? lhs_max : rhs_max;
	using policy = common_policy_t<lhs_policy, rhs_policy>;
public:
	// TODO: common_bound ?
	using type = bounded_integer<minimum, maximum, policy>;
};


template<typename T>
constexpr T min(T && value) noexcept {
	return value;
}
template<typename LHS, typename RHS, enable_if_t<
	(std::numeric_limits<LHS>::max() < std::numeric_limits<RHS>::min())
> = clang_dummy>
constexpr LHS min(LHS && lhs, RHS &&) noexcept {
	return lhs;
}
template<typename LHS, typename RHS, enable_if_t<
	(std::numeric_limits<RHS>::max() < std::numeric_limits<LHS>::min())
> = clang_dummy>
constexpr RHS min(LHS &&, RHS && rhs) noexcept {
	return rhs;
}
template<typename LHS, typename RHS, enable_if_t<
	!(std::numeric_limits<LHS>::max() < std::numeric_limits<RHS>::min()) and
	!(std::numeric_limits<RHS>::max() < std::numeric_limits<LHS>::min())
> = clang_dummy>
constexpr minmax_t<less, LHS, RHS> min(LHS && lhs, RHS && rhs) noexcept {
	using type = minmax_t<less, LHS, RHS>;
	return (lhs < rhs) ? type(lhs, non_check) : type(rhs, non_check);
}

template<typename LHS, typename RHS, typename... Others>
constexpr minmax_t<less, LHS, RHS, Others...> min(LHS && lhs, RHS && rhs, Others && ... others) noexcept {
	return min(std::forward<LHS>(lhs), min(std::forward<RHS>(rhs), std::forward<Others>(others)...));
}


template<typename T>
constexpr T max(T && value) noexcept {
	return value;
}
template<typename LHS, typename RHS, enable_if_t<
	(std::numeric_limits<LHS>::min() > std::numeric_limits<RHS>::max())
> = clang_dummy>
constexpr LHS max(LHS && lhs, RHS &&) noexcept {
	return lhs;
}
template<typename LHS, typename RHS, enable_if_t<
	(std::numeric_limits<RHS>::min() > std::numeric_limits<LHS>::max())
> = clang_dummy>
constexpr RHS max(LHS &&, RHS && rhs) noexcept {
	return rhs;
}
template<typename LHS, typename RHS, enable_if_t<
	!(std::numeric_limits<LHS>::max() < std::numeric_limits<RHS>::min()) and
	!(std::numeric_limits<RHS>::max() < std::numeric_limits<LHS>::min())
> = clang_dummy>
constexpr minmax_t<greater, LHS, RHS> max(LHS && lhs, RHS && rhs) noexcept {
	using type = minmax_t<greater, LHS, RHS>;
	return (lhs > rhs) ? type(lhs, non_check) : type(rhs, non_check);
}
template<typename LHS, typename RHS, typename... Others>
constexpr minmax_t<greater, LHS, RHS, Others...> max(LHS && lhs, RHS && rhs, Others && ... others) noexcept {
	return max(std::forward<LHS>(lhs), max(std::forward<RHS>(rhs), std::forward<Others>(others)...));
}

}	// namespace minmax
}	// namespace detail

template<typename... Integers>
constexpr auto min(Integers && ... integers) noexcept {
	return detail::minmax::min(make_bounded(std::forward<Integers>(integers))...);
}

template<typename... Integers>
constexpr auto max(Integers && ... integers) noexcept {
	return detail::minmax::max(make_bounded(std::forward<Integers>(integers))...);
}

}	// namespace bounded_integer
#endif	// BOUNDED_INTEGER_MINMAX_HPP_
