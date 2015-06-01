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


template<
	intmax_t lhs_min, intmax_t lhs_max,
	intmax_t rhs_min, intmax_t rhs_max
>
struct operator_range<lhs_min, lhs_max, rhs_min, rhs_max, modulus> {
private:
	static_assert(rhs_min != 0 or rhs_max != 0, "modulo is not defined for a divisor of zero.");
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
	static constexpr intmax_t greatest_divisor = (rhs_max < 0) ? rhs_min : ::bounded::min(rhs_min, -rhs_max, -1);
	static constexpr intmax_t least_divisor =
		(rhs_min > 0) ? -rhs_min :
		(rhs_max < 0) ? rhs_max :
		-1;
	static_assert(greatest_divisor < 0, "Got a positive value where a negative was expected.");
	static_assert(least_divisor < 0, "Got a positive value where a negative was expected.");

	struct result_type {
		intmax_t min;
		intmax_t max;
	};


	template<intmax_t minimum_dividend, intmax_t maximum_dividend>
	struct sign_free_value {
		static_assert(minimum_dividend <= 0, "Got a positive value where a negative was expected.");
		static_assert(maximum_dividend <= 0, "Got a positive value where a negative was expected.");
		static_assert(minimum_dividend >= maximum_dividend, "Range is inverted.");
	private:
		static constexpr auto initial_value = result_type{std::numeric_limits<intmax_t>::min(), std::numeric_limits<intmax_t>::max()};

		static constexpr auto combine(result_type lhs, result_type rhs) noexcept {
			return result_type{::bounded::max(lhs.min, rhs.min), ::bounded::min(lhs.max, rhs.max)};
		}

		static constexpr auto modulo_round(intmax_t divisor) noexcept {
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
			return (minimum_dividend / divisor == maximum_dividend / divisor) ?
				result_type{minimum_dividend % divisor, maximum_dividend % divisor} :
				result_type{0, divisor + 1};
		}

		static constexpr auto calculate(intmax_t divisor, result_type current) noexcept -> result_type {
			return ((current.min == -10 and current.max <= divisor + 1) or divisor > least_divisor) ?
				current :
				calculate(divisor + 1, combine(current, modulo_round(divisor)));
		}
	public:
		static constexpr auto value() noexcept {
			// I have the special case for -1 to avoid any possibility of
			// integer overflow from std::numeric_limits<intmax_t>::min() / -1
			return (greatest_divisor == -1) ? result_type{0, 0} : calculate(greatest_divisor, initial_value);
		}
		static_assert(value().min <= 0, "Got a positive value where a negative was expected.");
		static_assert(value().max <= 0, "Got a positive value where a negative was expected.");
		static_assert(value().min >= value().max, "Range is inverted.");
	};

	static constexpr intmax_t maybe_most_negative_dividend = lhs_min;
	static constexpr intmax_t maybe_least_negative_dividend = (lhs_max < 0) ? lhs_max : ::bounded::max(lhs_min, 0);
	static constexpr intmax_t maybe_most_positive_dividend = lhs_max;
	static constexpr intmax_t maybe_least_positive_dividend = (lhs_min > 0) ? lhs_min : ::bounded::min(lhs_max, 0);

	static constexpr bool has_positive_values = maybe_most_positive_dividend > 0;
	static constexpr bool has_negative_values = maybe_most_negative_dividend <= 0;

	static_assert(has_positive_values or has_negative_values, "The range must contain at least positive, negative, or zero values");

	// Avoid instantiating a template with unexpected values
	static constexpr intmax_t most_negative_dividend = maybe_most_negative_dividend * (has_negative_values ? 1 : 0);
	static constexpr intmax_t least_negative_dividend = maybe_least_negative_dividend * (has_negative_values ? 1 : 0);
	static constexpr intmax_t most_positive_dividend = maybe_most_positive_dividend * (has_positive_values ? 1 : 0);
	static constexpr intmax_t least_positive_dividend = maybe_least_positive_dividend * (has_positive_values ? 1 : 0);
	
	static constexpr result_type negative = sign_free_value<least_negative_dividend, most_negative_dividend>::value();
	static constexpr result_type positive = sign_free_value<-least_positive_dividend, -most_positive_dividend>::value();
public:
	static_assert(positive.min >= -std::numeric_limits<intmax_t>::max(), "Positive values out of range.");
	static_assert(positive.max >= -std::numeric_limits<intmax_t>::max(), "Positive values out of range.");
	static constexpr auto min() noexcept -> intmax_t {
		return !has_negative_values ? -positive.min : negative.max;
	}
	static constexpr auto max() noexcept -> intmax_t {
		return !has_positive_values ? negative.min : -positive.max;
	}
	static_assert(min() <= max(), "Range is inverted.");
};

}	// namespace detail
}	// namespace bounded

