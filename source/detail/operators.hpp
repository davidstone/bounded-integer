// Ranged integer operators
// Copyright (C) 2013 David Stone
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

#ifndef RANGED_INTEGER_OPERATORS_HPP_
#define RANGED_INTEGER_OPERATORS_HPP_

#include "class.hpp"

#include <cstdint>

// I'll have to revisit this. It currently invokes implementation-defined
// behavior by casting an unsigned value to a signed value.


// Addition
template<
	template<intmax_t, intmax_t> class result_overflow_policy,
	intmax_t lhs_min, intmax_t lhs_max, template<intmax_t, intmax_t> class lhs_overflow_policy,
	intmax_t rhs_min, intmax_t rhs_max, template<intmax_t, intmax_t> class rhs_overflow_policy,
	typename result_type = ranged_integer<lhs_min + rhs_min, lhs_max + rhs_max, result_overflow_policy>
>
constexpr result_type add(
	ranged_integer<lhs_min, lhs_max, lhs_overflow_policy> const lhs,
	ranged_integer<rhs_min, rhs_max, rhs_overflow_policy> const rhs
) noexcept {
	return result_type(static_cast<typename result_type::underlying_type>(lhs.value()) + static_cast<typename result_type::underlying_type>(rhs.value()), non_check);
}

template<
	intmax_t lhs_min, intmax_t lhs_max,
	intmax_t rhs_min, intmax_t rhs_max,
	template<intmax_t, intmax_t> class overflow_policy
>
constexpr auto operator+(
	ranged_integer<lhs_min, lhs_max, overflow_policy> const lhs,
	ranged_integer<rhs_min, rhs_max, overflow_policy> const rhs
) noexcept -> decltype(add<overflow_policy>(lhs, rhs)) {
	return add<overflow_policy>(lhs, rhs);
}



// Subtraction
template<
	template<intmax_t, intmax_t> class result_overflow_policy,
	intmax_t lhs_min, intmax_t lhs_max, template<intmax_t, intmax_t> class lhs_overflow_policy,
	intmax_t rhs_min, intmax_t rhs_max, template<intmax_t, intmax_t> class rhs_overflow_policy,
	typename result_type = ranged_integer<lhs_min - rhs_max, lhs_max - rhs_min, result_overflow_policy>
>
constexpr result_type subtract(
	ranged_integer<lhs_min, lhs_max, lhs_overflow_policy> const lhs,
	ranged_integer<rhs_min, rhs_max, rhs_overflow_policy> const rhs
) noexcept {
	return result_type(lhs.value() - rhs.value(), non_check);
}

template<
	intmax_t lhs_min, intmax_t lhs_max,
	intmax_t rhs_min, intmax_t rhs_max,
	template<intmax_t, intmax_t> class overflow_policy
>
constexpr auto operator-(
	ranged_integer<lhs_min, lhs_max, overflow_policy> const lhs,
	ranged_integer<rhs_min, rhs_max, overflow_policy> const rhs
) noexcept -> decltype(subtract<overflow_policy>(lhs, rhs)) {
	return subtract<overflow_policy>(lhs, rhs);
}



// Multiplication
namespace detail {
constexpr intmax_t min(intmax_t lhs, intmax_t rhs) noexcept {
	return (lhs < rhs) ? lhs : rhs;
}
constexpr intmax_t max(intmax_t lhs, intmax_t rhs) noexcept {
	return (lhs >= rhs) ? lhs : rhs;
}
template<intmax_t lhs_min, intmax_t lhs_max, intmax_t rhs_min, intmax_t rhs_max>
class product_range {
private:
	static constexpr auto p0 = lhs_min * rhs_min;
	static constexpr auto p1 = lhs_min * rhs_max;
	static constexpr auto p2 = lhs_max * rhs_min;
	static constexpr auto p3 = lhs_max * rhs_max;
public:
	static constexpr auto min_product = min(p0, min(p1, min(p2, p3)));
	static constexpr auto max_product = max(p0, max(p1, max(p2, p3)));
};
}	// namespace detail

template<
	template<intmax_t, intmax_t> class result_overflow_policy,
	intmax_t lhs_min, intmax_t lhs_max, template<intmax_t, intmax_t> class lhs_overflow_policy,
	intmax_t rhs_min, intmax_t rhs_max, template<intmax_t, intmax_t> class rhs_overflow_policy,
	typename product_range_t = detail::product_range<lhs_min, lhs_max, rhs_min, rhs_max>,
	typename result_type = ranged_integer<product_range_t::min_product, product_range_t::max_product, result_overflow_policy>
>
constexpr result_type multiply(
	ranged_integer<lhs_min, lhs_max, lhs_overflow_policy> const lhs,
	ranged_integer<rhs_min, rhs_max, rhs_overflow_policy> const rhs
) noexcept {
	return result_type(static_cast<typename result_type::underlying_type>(lhs.value()) * static_cast<typename result_type::underlying_type>(rhs.value()), non_check);
}

template<
	intmax_t lhs_min, intmax_t lhs_max,
	intmax_t rhs_min, intmax_t rhs_max,
	template<intmax_t, intmax_t> class overflow_policy
>
constexpr auto operator*(
	ranged_integer<lhs_min, lhs_max, overflow_policy> const lhs,
	ranged_integer<rhs_min, rhs_max, overflow_policy> const rhs
) noexcept -> decltype(multiply<overflow_policy>(lhs, rhs)) {
	return multiply<overflow_policy>(lhs, rhs);
}



// Division
namespace detail {
template<intmax_t lhs_min, intmax_t lhs_max, intmax_t rhs_min, intmax_t rhs_max>
class quotient_range {
private:
	// If 1 falls within the range, that is the least positive divisor. The
	// other options are a range that are entirely positive, in which case I
	// want to return the least value, or the range is entirely negative or
	// zero, in which case I pick the greatest absolute value (which is the
	// minimum) to that the 'positive' divisor is not selected in a later step.
	// We can use similar logic for greatest_negative_divisor.
	static constexpr auto least_positive_divisor = (rhs_min <= 1 and 1 <= rhs_max) ? 1 : rhs_min;
	static constexpr auto greatest_negative_divisor = (rhs_min <= -1 and -1 <= rhs_max) ? -1 : rhs_max;
	static constexpr auto g0 = lhs_min / least_positive_divisor;
	static constexpr auto g1 = lhs_min / greatest_negative_divisor;
	static constexpr auto g2 = lhs_max / least_positive_divisor;
	static constexpr auto g3 = lhs_max / greatest_negative_divisor;

	static constexpr auto l0 = lhs_min / rhs_min;
	static constexpr auto l1 = lhs_min / rhs_max;
	static constexpr auto l2 = lhs_max / rhs_min;
	static constexpr auto l3 = lhs_max / rhs_max;
public:
	static constexpr auto min_quotient = min(min(l0, min(l1, min(l2, l3))), min(g0, min(g1, min(g2, g3))));
	static constexpr auto max_quotient = max(g0, max(g1, max(g2, g3)));
};

}	// namespace detail

template<
	template<intmax_t, intmax_t> class result_overflow_policy,
	intmax_t lhs_min, intmax_t lhs_max, template<intmax_t, intmax_t> class lhs_overflow_policy,
	intmax_t rhs_min, intmax_t rhs_max, template<intmax_t, intmax_t> class rhs_overflow_policy,
	typename quotient_range_t = detail::quotient_range<lhs_min, lhs_max, rhs_min, rhs_max>,
	typename result_type = ranged_integer<quotient_range_t::min_quotient, quotient_range_t::max_quotient, result_overflow_policy>
>
constexpr result_type divide(
	ranged_integer<lhs_min, lhs_max, lhs_overflow_policy> const lhs,
	ranged_integer<rhs_min, rhs_max, rhs_overflow_policy> const rhs
) noexcept {
	return result_type(lhs.value() / rhs.value(), non_check);
}

template<
	intmax_t lhs_min, intmax_t lhs_max,
	intmax_t rhs_min, intmax_t rhs_max,
	template<intmax_t, intmax_t> class overflow_policy
>
constexpr auto operator/(
	ranged_integer<lhs_min, lhs_max, overflow_policy> const lhs,
	ranged_integer<rhs_min, rhs_max, overflow_policy> const rhs
) noexcept -> decltype(divide<overflow_policy>(lhs, rhs)) {
	return divide<overflow_policy>(lhs, rhs);
}



// Unary minus

// Cannot be marked noexcept because -minimum might not exist
template<
	template<intmax_t, intmax_t> class result_overflow_policy,
	intmax_t minimum, intmax_t maximum, template<intmax_t, intmax_t> class overflow_policy,
	typename result_type = ranged_integer<-maximum, -minimum, result_overflow_policy>
>
constexpr result_type negate(ranged_integer<minimum, maximum, overflow_policy> const value) {
	return result_type(-static_cast<typename result_type::underlying_type>(value), non_check);
}

template<intmax_t minimum, intmax_t maximum, template<intmax_t, intmax_t> class overflow_policy>
constexpr auto operator-(ranged_integer<minimum, maximum, overflow_policy> const value) -> decltype(negate<overflow_policy>(value)) {
	return negate<overflow_policy>(value);
}



// In my original implementation, I wrote these as comparisons between a
// ranged_integer and its underlying type. However, this actually makes
// relational operators much less useful than they otherwise could be. For
// instance, if we have a ranged_integer with bounds [-100, 100], we might want
// to compare that with an int64_t. If we have to first convert the value to the
// underlying type (likely int8_t), then we are likely to invoke undefined
// behavior. Instead, the comparison function should accept a much wider array
// of types and convert up to the common type.

// Equality
template<
	intmax_t lhs_min, intmax_t lhs_max, template<intmax_t, intmax_t> class lhs_overflow,
	intmax_t rhs_min, intmax_t rhs_max, template<intmax_t, intmax_t> class rhs_overflow
>
constexpr bool operator==(ranged_integer<lhs_min, lhs_max, lhs_overflow> const lhs, ranged_integer<rhs_min, rhs_max, rhs_overflow> const rhs) noexcept {
	// This doesn't properly handle signed / unsigned conversions
	return lhs.value() == rhs.value();
}

template<
	intmax_t lhs_min, intmax_t lhs_max, template<intmax_t, intmax_t> class lhs_overflow,
	intmax_t rhs_min, intmax_t rhs_max, template<intmax_t, intmax_t> class rhs_overflow
>
constexpr bool operator!=(ranged_integer<lhs_min, lhs_max, lhs_overflow> const lhs, ranged_integer<rhs_min, rhs_max, rhs_overflow> const rhs) noexcept {
	return !(lhs == rhs);
}



// Equality with built-ins

template<intmax_t minimum, intmax_t maximum, template<intmax_t, intmax_t> class overflow_policy, typename integer, enable_if_t<std::is_integral<integer>::value>...>
constexpr bool operator==(ranged_integer<minimum, maximum, overflow_policy> const lhs, integer const rhs) noexcept {
	return lhs.value() == rhs;
}
template<intmax_t minimum, intmax_t maximum, template<intmax_t, intmax_t> class overflow_policy, typename integer, enable_if_t<std::is_integral<integer>::value>...>
constexpr bool operator==(integer const lhs, ranged_integer<minimum, maximum, overflow_policy> const rhs) noexcept {
	return lhs == rhs.value();
}
template<intmax_t minimum, intmax_t maximum, template<intmax_t, intmax_t> class overflow_policy, typename integer, enable_if_t<std::is_integral<integer>::value>...>
constexpr bool operator!=(ranged_integer<minimum, maximum, overflow_policy> const lhs, integer const rhs) noexcept {
	return !(lhs == rhs);
}
template<intmax_t minimum, intmax_t maximum, template<intmax_t, intmax_t> class overflow_policy, typename integer, enable_if_t<std::is_integral<integer>::value>...>
constexpr bool operator!=(integer const lhs, ranged_integer<minimum, maximum, overflow_policy> const rhs) noexcept {
	return !(lhs == rhs);
}



// Relational operators

template<
	intmax_t lhs_min, intmax_t lhs_max, template<intmax_t, intmax_t> class lhs_overflow,
	intmax_t rhs_min, intmax_t rhs_max, template<intmax_t, intmax_t> class rhs_overflow
>
constexpr bool operator<(ranged_integer<lhs_min, lhs_max, lhs_overflow> const lhs, ranged_integer<rhs_min, rhs_max, rhs_overflow> const rhs) noexcept {
	// This doesn't properly handle signed / unsigned conversions
	return lhs.value() < rhs.value();
}

template<
	intmax_t lhs_min, intmax_t lhs_max, template<intmax_t, intmax_t> class lhs_overflow,
	intmax_t rhs_min, intmax_t rhs_max, template<intmax_t, intmax_t> class rhs_overflow
>
constexpr bool operator>(ranged_integer<lhs_min, lhs_max, lhs_overflow> const lhs, ranged_integer<rhs_min, rhs_max, rhs_overflow> const rhs) noexcept {
	// This doesn't properly handle signed / unsigned conversions
	return lhs.value() > rhs.value();
}

template<
	intmax_t lhs_min, intmax_t lhs_max, template<intmax_t, intmax_t> class lhs_overflow,
	intmax_t rhs_min, intmax_t rhs_max, template<intmax_t, intmax_t> class rhs_overflow
>
constexpr bool operator<=(ranged_integer<lhs_min, lhs_max, lhs_overflow> const lhs, ranged_integer<rhs_min, rhs_max, rhs_overflow> const rhs) noexcept {
	// This doesn't properly handle signed / unsigned conversions
	return lhs.value() <= rhs.value();
}

template<
	intmax_t lhs_min, intmax_t lhs_max, template<intmax_t, intmax_t> class lhs_overflow,
	intmax_t rhs_min, intmax_t rhs_max, template<intmax_t, intmax_t> class rhs_overflow
>
constexpr bool operator>=(ranged_integer<lhs_min, lhs_max, lhs_overflow> const lhs, ranged_integer<rhs_min, rhs_max, rhs_overflow> const rhs) noexcept {
	// This doesn't properly handle signed / unsigned conversions
	return lhs.value() >= rhs.value();
}



// Relational operators with built-ins

template<intmax_t minimum, intmax_t maximum, template<intmax_t, intmax_t> class overflow_policy, typename integer, enable_if_t<std::is_integral<integer>::value>...>
constexpr bool operator<(ranged_integer<minimum, maximum, overflow_policy> const lhs, integer const rhs) noexcept {
	return lhs.value() < rhs;
}
template<intmax_t minimum, intmax_t maximum, template<intmax_t, intmax_t> class overflow_policy, typename integer, enable_if_t<std::is_integral<integer>::value>...>
constexpr bool operator<(integer const lhs, ranged_integer<minimum, maximum, overflow_policy> const rhs) noexcept {
	return lhs < rhs.value();
}

template<intmax_t minimum, intmax_t maximum, template<intmax_t, intmax_t> class overflow_policy, typename integer, enable_if_t<std::is_integral<integer>::value>...>
constexpr bool operator>(ranged_integer<minimum, maximum, overflow_policy> const lhs, integer const rhs) noexcept {
	return lhs.value() > rhs;
}
template<intmax_t minimum, intmax_t maximum, template<intmax_t, intmax_t> class overflow_policy, typename integer, enable_if_t<std::is_integral<integer>::value>...>
constexpr bool operator>(integer const lhs, ranged_integer<minimum, maximum, overflow_policy> const rhs) noexcept {
	return lhs > rhs.value();
}

template<intmax_t minimum, intmax_t maximum, template<intmax_t, intmax_t> class overflow_policy, typename integer, enable_if_t<std::is_integral<integer>::value>...>
constexpr bool operator<=(ranged_integer<minimum, maximum, overflow_policy> const lhs, integer const rhs) noexcept {
	return lhs.value() <= rhs;
}
template<intmax_t minimum, intmax_t maximum, template<intmax_t, intmax_t> class overflow_policy, typename integer, enable_if_t<std::is_integral<integer>::value>...>
constexpr bool operator<=(integer const lhs, ranged_integer<minimum, maximum, overflow_policy> const rhs) noexcept {
	return lhs <= rhs.value();
}

template<intmax_t minimum, intmax_t maximum, template<intmax_t, intmax_t> class overflow_policy, typename integer, enable_if_t<std::is_integral<integer>::value>...>
constexpr bool operator>=(ranged_integer<minimum, maximum, overflow_policy> const lhs, integer const rhs) noexcept {
	return lhs.value() >= rhs;
}
template<intmax_t minimum, intmax_t maximum, template<intmax_t, intmax_t> class overflow_policy, typename integer, enable_if_t<std::is_integral<integer>::value>...>
constexpr bool operator>=(integer const lhs, ranged_integer<minimum, maximum, overflow_policy> const rhs) noexcept {
	return lhs >= rhs.value();
}


#endif	// RANGED_INTEGER_OPERATORS_HPP_
