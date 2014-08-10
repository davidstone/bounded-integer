// result_type of arithmetic operations
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

#ifndef BOUNDED_INTEGER_ARITHMETIC_RESULT_TYPE_HPP_
#define BOUNDED_INTEGER_ARITHMETIC_RESULT_TYPE_HPP_

#include "../forward_declaration.hpp"
#include "../minmax.hpp"
#include "../numeric_limits.hpp"
#include "../policy/common_policy.hpp"

#include <cstdint>
#include <utility>

namespace bounded {
namespace detail {

// We have to define our own function objects rather than using the classes from
// <functional> because we need constexpr

class plus {
public:
	template<typename LHS, typename RHS>
	constexpr auto operator()(LHS && lhs, RHS && rhs) const noexcept {
		return std::forward<LHS>(lhs) + std::forward<RHS>(rhs);
	}
};
class minus {
public:
	template<typename LHS, typename RHS>
	constexpr auto operator()(LHS && lhs, RHS && rhs) const noexcept {
		return std::forward<LHS>(lhs) - std::forward<RHS>(rhs);
	}
};
class multiplies {
public:
	template<typename LHS, typename RHS>
	constexpr auto operator()(LHS && lhs, RHS && rhs) const noexcept {
		return std::forward<LHS>(lhs) * std::forward<RHS>(rhs);
	}
};
class divides {
public:
	template<typename LHS, typename RHS>
	constexpr auto operator()(LHS && lhs, RHS && rhs) const noexcept {
		static_assert(noexcept(std::forward<LHS>(lhs) / std::forward<RHS>(rhs)), "Division can throw exceptions."); 
		return std::forward<LHS>(lhs) / std::forward<RHS>(rhs);
	}
};
class modulus {
public:
	template<typename LHS, typename RHS>
	constexpr auto operator()(LHS && lhs, RHS && rhs) const noexcept {
		static_assert(noexcept(std::forward<LHS>(lhs) % std::forward<RHS>(rhs)), "Modulus can throw exceptions."); 
		return std::forward<LHS>(lhs) % std::forward<RHS>(rhs);
	}
};
class left_shift {
public:
	template<typename LHS, typename RHS>
	constexpr auto operator()(LHS && lhs, RHS && rhs) const noexcept {
		static_assert(noexcept(std::forward<LHS>(lhs) << std::forward<RHS>(rhs)), "Left shift can throw exceptions."); 
		return std::forward<LHS>(lhs) << std::forward<RHS>(rhs);
	}
};
class right_shift {
public:
	template<typename LHS, typename RHS>
	constexpr auto operator()(LHS && lhs, RHS && rhs) const noexcept {
		static_assert(noexcept(std::forward<LHS>(lhs) >> std::forward<RHS>(rhs)), "Right shift can throw exceptions."); 
		return std::forward<LHS>(lhs) >> std::forward<RHS>(rhs);
	}
};

template<
	intmax_t lhs_min, intmax_t lhs_max,
	intmax_t rhs_min, intmax_t rhs_max,
	typename Operator
>
class operator_range {
// TODO
};

template<
	intmax_t lhs_min, intmax_t lhs_max,
	intmax_t rhs_min, intmax_t rhs_max
>
class operator_range<lhs_min, lhs_max, rhs_min, rhs_max, plus> {
public:
	static constexpr auto min() noexcept -> intmax_t {
		return lhs_min + rhs_min;
	}
	static constexpr auto max() noexcept -> intmax_t {
		return lhs_max + rhs_max;
	}
	static_assert(min() <= max(), "Range is inverted.");
};

template<
	intmax_t lhs_min, intmax_t lhs_max,
	intmax_t rhs_min, intmax_t rhs_max
>
class operator_range<lhs_min, lhs_max, rhs_min, rhs_max, minus> {
public:
	static constexpr auto min() noexcept -> intmax_t {
		return lhs_min - rhs_max;
	}
	static constexpr auto max() noexcept -> intmax_t {
		return lhs_max - rhs_min;
	}
	static_assert(min() <= max(), "Range is inverted.");
};

template<
	intmax_t lhs_min, intmax_t lhs_max,
	intmax_t rhs_min, intmax_t rhs_max
>
class operator_range<lhs_min, lhs_max, rhs_min, rhs_max, multiplies> {
private:
	static constexpr intmax_t p0 = lhs_min * rhs_min;
	static constexpr intmax_t p1 = lhs_min * rhs_max;
	static constexpr intmax_t p2 = lhs_max * rhs_min;
	static constexpr intmax_t p3 = lhs_max * rhs_max;
public:
	static constexpr auto min() noexcept -> intmax_t {
		return ::bounded::min(p0, p1, p2, p3);
	}
	static constexpr auto max() noexcept -> intmax_t {
		return ::bounded::max(p0, p1, p2, p3);
	}
	static_assert(min() <= max(), "Range is inverted.");
};

template<
	intmax_t lhs_min, intmax_t lhs_max,
	intmax_t rhs_min, intmax_t rhs_max
>
class operator_range<lhs_min, lhs_max, rhs_min, rhs_max, divides> {
private:
	static_assert(rhs_min != 0 or rhs_max != 0, "division is not defined for a divisor of zero.");
	// If 1 falls within the range, that is the least positive divisor. The
	// other options are a range that are entirely positive, in which case I
	// want to return the least value, or the range is entirely negative or
	// zero, in which case I pick the greatest absolute value (which is the
	// minimum) so that the 'positive' divisor is not selected in a later step.
	// We can use similar logic for greatest_negative_divisor.
	static constexpr intmax_t least_positive_divisor = (rhs_min <= 1 and 1 <= rhs_max) ? 1 : rhs_min;
	static constexpr intmax_t greatest_negative_divisor = (rhs_min <= -1 and -1 <= rhs_max) ? -1 : rhs_max;
	static constexpr intmax_t g0 = lhs_min / least_positive_divisor;
	static constexpr intmax_t g1 = lhs_min / greatest_negative_divisor;
	static constexpr intmax_t g2 = lhs_max / least_positive_divisor;
	static constexpr intmax_t g3 = lhs_max / greatest_negative_divisor;

	static constexpr intmax_t l0 = lhs_min / rhs_min;
	static constexpr intmax_t l1 = lhs_min / rhs_max;
	static constexpr intmax_t l2 = lhs_max / rhs_min;
	static constexpr intmax_t l3 = lhs_max / rhs_max;
public:
	static constexpr auto min() noexcept -> intmax_t {
		return ::bounded::min(l0, l1, l2, l3, g0, g1, g2, g3);
	}
	static constexpr auto max() noexcept -> intmax_t {
		return ::bounded::max(g0, g1, g2, g3);
	}
	static_assert(min() <= max(), "Range is inverted.");
};

template<
	intmax_t lhs_min, intmax_t lhs_max,
	intmax_t rhs_min, intmax_t rhs_max
>
class operator_range<lhs_min, lhs_max, rhs_min, rhs_max, modulus> {
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
	using result_type = std::pair<intmax_t, intmax_t>;

	template<intmax_t minimum_dividend, intmax_t maximum_dividend>
	class sign_free_value {
	public:
		static_assert(minimum_dividend <= 0, "Got a positive value where a negative was expected.");
		static_assert(maximum_dividend <= 0, "Got a positive value where a negative was expected.");
		static_assert(minimum_dividend >= maximum_dividend, "Range is inverted.");
	private:
		static constexpr auto initial_value = result_type(std::numeric_limits<intmax_t>::min(), std::numeric_limits<intmax_t>::max());

		static constexpr auto combine(result_type lhs, result_type rhs) noexcept {
			return result_type(::bounded::max(lhs.first, rhs.first), ::bounded::min(lhs.second, rhs.second));
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
				result_type(minimum_dividend % divisor, maximum_dividend % divisor) :
				result_type(0, divisor + 1);
		}

		static constexpr auto calculate(intmax_t divisor, result_type current) noexcept -> result_type {
			return ((current.first == 0 and current.second <= divisor + 1) or divisor > least_divisor) ?
				current :
				calculate(divisor + 1, combine(current, modulo_round(divisor)));
		}
	public:
		static constexpr auto value() noexcept {
			// I have the special case for -1 to avoid any possibility of
			// integer overflow from std::numeric_limits<intmax_t>::min() / -1
			return (greatest_divisor == -1) ? result_type(0, 0) : calculate(greatest_divisor, initial_value);
		}
		static_assert(value().first <= 0, "Got a positive value where a negative was expected.");
		static_assert(value().second <= 0, "Got a positive value where a negative was expected.");
		static_assert(value().first >= value().second, "Range is inverted.");
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
	static_assert(positive.first >= -std::numeric_limits<intmax_t>::max(), "Positive values out of range.");
	static_assert(positive.second >= -std::numeric_limits<intmax_t>::max(), "Positive values out of range.");
	static constexpr auto min() noexcept -> intmax_t {
		return
			!has_positive_values ? negative.second :
			!has_negative_values ? -positive.first :
			::bounded::min(negative.second, -positive.first);
	}
	static constexpr auto max() noexcept -> intmax_t {
		return
			!has_positive_values ? negative.first :
			!has_negative_values ? -positive.second :
			::bounded::max(negative.first, -positive.second);
	}
	static_assert(min() <= max(), "Range is inverted.");
};

template<
	intmax_t lhs_min, intmax_t lhs_max,
	intmax_t rhs_min, intmax_t rhs_max
>
class operator_range<lhs_min, lhs_max, rhs_min, rhs_max, left_shift> {
public:
	static_assert(lhs_min >= 0 and rhs_min >= 0, "Left shift not defined for negative values.");
	static_assert(rhs_max <= std::numeric_limits<intmax_t>::digits, "Cannot left shift >= width of intmax_t.");
	static constexpr auto min() noexcept -> intmax_t {
		return lhs_min << rhs_min;
	}
	static constexpr auto max() noexcept -> intmax_t {
		return lhs_max << rhs_max;
	}
	static_assert(min() <= max(), "Range is inverted.");
};

template<
	intmax_t lhs_min, intmax_t lhs_max,
	intmax_t rhs_min, intmax_t rhs_max
>
class operator_range<lhs_min, lhs_max, rhs_min, rhs_max, right_shift> {
public:
	static_assert(lhs_min >= 0 and rhs_min >= 0, "Right shift not defined for negative values.");
	static_assert(rhs_max <= std::numeric_limits<intmax_t>::digits, "Cannot right shift >= width of intmax_t.");
	static constexpr auto min() noexcept -> intmax_t {
		return lhs_min >> rhs_max;
	}
	static constexpr auto max() noexcept -> intmax_t {
		return lhs_max >> rhs_min;
	}
	static_assert(min() <= max(), "Range is inverted.");
};



template<
	intmax_t lhs_min, intmax_t lhs_max, typename lhs_overflow,
	intmax_t rhs_min, intmax_t rhs_max, typename rhs_overflow,
	storage_type storage,
	typename Operator
>
using operator_result = integer<
	detail::operator_range<lhs_min, lhs_max, rhs_min, rhs_max, Operator>::min(),
	detail::operator_range<lhs_min, lhs_max, rhs_min, rhs_max, Operator>::max(),
	common_policy_t<lhs_overflow, rhs_overflow>,
	storage
>;

template<intmax_t minimum, intmax_t maximum, typename overflow_policy, storage_type storage>
using unary_minus_result = integer<-maximum, -minimum, overflow_policy, storage>;


}	// namespace detail
}	// namespace bounded

#endif	// BOUNDED_INTEGER_ARITHMETIC_RESULT_TYPE_HPP_
