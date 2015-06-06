// result_type of modulus
// Copyright (C) 2015 David Stone
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

#pragma once

#include "base.hpp"

#include "../../minmax.hpp"

#include <limits>
#include <utility>

namespace bounded {
namespace detail {

struct modulus {
	template<typename LHS, typename RHS>
	constexpr auto operator()(LHS && lhs, RHS && rhs) const noexcept {
		static_assert(noexcept(std::forward<LHS>(lhs) % std::forward<RHS>(rhs)), "Modulus can throw exceptions."); 
		return std::forward<LHS>(lhs) % std::forward<RHS>(rhs);
	}
};

namespace modulus_detail {

struct min_max {
	constexpr min_max(intmax_t min_, intmax_t max_) noexcept:
		min(min_),
		max(max_) {
	}
	intmax_t min;
	intmax_t max;
};

constexpr auto overlap(min_max lhs, min_max rhs) noexcept {
	return min_max(max(lhs.min, rhs.min), min(lhs.max, rhs.max));
}

constexpr auto modulo_round(min_max dividend, intmax_t divisor) noexcept {
	// When we divide both ends of the dividend by a particular value in
	// the range of the divisor there are two possibilities:
	//
	// If they are not equal, then that means that the periodic modulo
	// function resets to 0 somewhere in that range. The greatest value
	// is immediately before this resets, but we do not have to find
	// where in the range it is. The magnitude will be one less than the
	// magnitude of the divisor. The least value is obviously 0, as we
	// are ignoring the sign of the dividend for these calculations.
	//
	// If they are equal, the maximum value is at the top end of the
	// range. The function never reset, and it increases by one for each
	// increase in the dividend until it resets. The maximum value is
	// the point at which there were the most increases. The minimum
	// value is the point at which there were the fewest increases.
	return (dividend.min / divisor == dividend.max / divisor) ?
		min_max(dividend.min % divisor, dividend.max % divisor) :
		min_max(0, divisor + 1);
}

constexpr auto sign_free_value(min_max dividend, min_max divisor) noexcept {
	// I have the special case for -1 to avoid any possibility of
	// integer overflow from std::numeric_limits<intmax_t>::min() / -1
	if (divisor.max == -1) {
		return min_max(0, 0);
	}

	auto current = min_max(std::numeric_limits<intmax_t>::min(), std::numeric_limits<intmax_t>::max());
	while (!(current.min == 0 and current.max <= divisor.max + 1) and divisor.max <= divisor.min) {
		current = overlap(current, modulo_round(dividend, divisor.max));
		++divisor.max;
	}
	return current;
}

}	// namespace modulus_detail

template<
	intmax_t lhs_min, intmax_t lhs_max,
	intmax_t rhs_min, intmax_t rhs_max
>
struct operator_range<lhs_min, lhs_max, rhs_min, rhs_max, modulus> {
private:
	static_assert(rhs_min != 0 or rhs_max != 0, "modulo is not defined for a divisor of zero.");
	static_assert(lhs_min <= lhs_max, "Range inverted.");
	static_assert(rhs_min <= rhs_max, "Range inverted.");
	// The sign of the result is equal to the sign of the lhs. The sign of the
	// rhs does not matter.
	//
	// We must be careful when negating due to the possibility of overflow.
	//
	// This operates on negatives until the end to avoid overflow when
	// value == std::numeric_limits<intmax_t>::min()
	// Therefore, all temporaries should be considered to have an implicit
	// "magnitude" and "negative" in their name.
	//
	// The divisor range cannot terminate on a 0 since that is an invalid value.
	static constexpr auto divisor = modulus_detail::min_max(
		(rhs_min > 0) ? -rhs_min : (rhs_max < 0) ? rhs_max : -1,
		(rhs_max < 0) ? rhs_min : bounded::min(rhs_min, -rhs_max)
	);
	static_assert(divisor.max < 0, "Got a positive value where a negative was expected.");
	static_assert(divisor.min < 0, "Got a positive value where a negative was expected.");

	// Avoid instantiating a template with unexpected values
	static constexpr auto negative_dividend = modulus_detail::min_max(bounded::min(lhs_max, 0), bounded::min(lhs_min, 0));
	static constexpr auto positive_dividend = modulus_detail::min_max(-bounded::max(lhs_min, 0), -bounded::max(lhs_max, 0));
	
	static constexpr auto negative = modulus_detail::sign_free_value(negative_dividend, divisor);
	static constexpr auto positive = modulus_detail::sign_free_value(positive_dividend, divisor);
public:
	static_assert(positive.min >= -std::numeric_limits<intmax_t>::max(), "Positive values out of range.");
	static_assert(positive.max >= -std::numeric_limits<intmax_t>::max(), "Positive values out of range.");
	static constexpr auto min() noexcept -> intmax_t {
		constexpr bool has_negative_values = lhs_min <= 0;
		return has_negative_values ? negative.max : -positive.min;
	}
	static constexpr auto max() noexcept -> intmax_t {
		constexpr bool has_positive_values = lhs_max > 0;
		return has_positive_values ? -positive.max : negative.min;
	}
	static_assert(min() <= max(), "Range is inverted.");
};

}	// namespace detail
}	// namespace bounded

