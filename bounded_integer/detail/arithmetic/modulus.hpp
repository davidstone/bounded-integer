// Modulus operator overload when each argument is a bounded::integer
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

#include "../minmax.hpp"

#include <functional>
#include <limits>
#include <utility>

namespace bounded {
namespace detail {

template<typename LHS, typename RHS>
constexpr auto overlap(LHS const & lhs, RHS const & rhs) noexcept {
	return min_max(max(lhs.min, rhs.min), min(lhs.max, rhs.max));
}

template<typename Dividend, typename Divisor>
constexpr auto modulo_round(Dividend const & dividend, Divisor const & divisor) noexcept {
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
		min_max(static_cast<Divisor>(0), divisor + 1);
}

template<typename Dividend, typename Divisor>
constexpr auto sign_free_value(Dividend const & dividend, Divisor divisor) noexcept {
	// I have the special case for -1 to avoid any possibility of
	// integer overflow from std::numeric_limits<intmax_t>::min() / -1
	if (divisor.max == -1) {
		return min_max(static_cast<intmax_t>(0), static_cast<intmax_t>(0));
	}

	auto current = min_max(std::numeric_limits<intmax_t>::min(), std::numeric_limits<intmax_t>::max());
	while (!(current.min == 0 and current.max <= divisor.max + 1) and divisor.max <= divisor.min) {
		current = overlap(current, modulo_round(dividend, divisor.max));
		++divisor.max;
	}
	return current;
}

template<typename LHS, typename RHS>
constexpr auto operator_range(LHS const & lhs, RHS const & rhs, std::modulus<>) noexcept {
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
	auto const divisor = min_max(
		(rhs.min > 0) ? -rhs.min : (rhs.max < 0) ? rhs.max : -1,
		(rhs.max < 0) ? rhs.min : min(rhs.min, -rhs.max)
	);

	// Avoid instantiating a template with unexpected values
	auto const negative_dividend = min_max(bounded::min(lhs.max, 0), bounded::min(lhs.min, 0));
	auto const positive_dividend = min_max(-bounded::max(lhs.min, 0), -bounded::max(lhs.max, 0));
	
	auto const negative = sign_free_value(negative_dividend, divisor);
	auto const positive = sign_free_value(positive_dividend, divisor);

	auto const has_negative_values = lhs.min <= 0;
	auto const has_positive_values = lhs.max > 0;

	return min_max(
		has_negative_values ? negative.max : -positive.min,
		has_positive_values ? -positive.max : negative.min
	);
}

}	// namespace detail

BOUNDED_INTEGER_OPERATOR_OVERLOADS(%, std::modulus<>)

}	// namespace bounded

