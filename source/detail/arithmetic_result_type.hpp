// result_type of arithmetic operations
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

#ifndef RANGED_INTEGER_ARITHMETIC_RESULT_TYPE_HPP_
#define RANGED_INTEGER_ARITHMETIC_RESULT_TYPE_HPP_

#include "common_type.hpp"
#include "enable_if.hpp"
#include "forward_declaration.hpp"
#include "make_ranged.hpp"
#include "minmax.hpp"

#include <cstdint>

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
	using common = common_type_t<result_type, ranged_integer<lhs_min, lhs_max, lhs_overflow_policy>, ranged_integer<rhs_min, rhs_max, rhs_overflow_policy>>;
	return result_type(static_cast<common>(lhs).value() + static_cast<common>(rhs).value(), non_check);
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

// Addition with built-ins
template<
	intmax_t lhs_min, intmax_t lhs_max,
	typename integer,
	template<intmax_t, intmax_t> class overflow_policy,
	enable_if_t<std::is_integral<integer>::value>...
>
constexpr auto operator+(
	ranged_integer<lhs_min, lhs_max, overflow_policy> const lhs,
	integer const rhs
) noexcept -> decltype(add<overflow_policy>(lhs, make_ranged(rhs))) {
	return add<overflow_policy>(lhs, make_ranged(rhs));
}
template<
	typename integer,
	intmax_t rhs_min, intmax_t rhs_max,
	template<intmax_t, intmax_t> class overflow_policy,
	enable_if_t<std::is_integral<integer>::value>...
>
constexpr auto operator+(
	integer const lhs,
	ranged_integer<rhs_min, rhs_max, overflow_policy> const rhs
) noexcept -> decltype(add<overflow_policy>(make_ranged(lhs), rhs)) {
	return add<overflow_policy>(make_ranged(lhs), rhs);
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
	using common = common_type_t<result_type, ranged_integer<lhs_min, lhs_max, lhs_overflow_policy>, ranged_integer<rhs_min, rhs_max, rhs_overflow_policy>>;
	return result_type(static_cast<common>(lhs).value() - static_cast<common>(rhs).value(), non_check);
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

// Subtraction with built-ins
template<
	intmax_t lhs_min, intmax_t lhs_max,
	typename integer,
	template<intmax_t, intmax_t> class overflow_policy,
	enable_if_t<std::is_integral<integer>::value>...
>
constexpr auto operator-(
	ranged_integer<lhs_min, lhs_max, overflow_policy> const lhs,
	integer const rhs
) noexcept -> decltype(subtract<overflow_policy>(lhs, make_ranged(rhs))) {
	return subtract<overflow_policy>(lhs, make_ranged(rhs));
}
template<
	typename integer,
	intmax_t rhs_min, intmax_t rhs_max,
	template<intmax_t, intmax_t> class overflow_policy,
	enable_if_t<std::is_integral<integer>::value>...
>
constexpr auto operator-(
	integer const lhs,
	ranged_integer<rhs_min, rhs_max, overflow_policy> const rhs
) noexcept -> decltype(subtract<overflow_policy>(make_ranged(lhs), rhs)) {
	return subtract<overflow_policy>(make_ranged(lhs), rhs);
}



// Multiplication
namespace detail {
template<intmax_t lhs_min, intmax_t lhs_max, intmax_t rhs_min, intmax_t rhs_max>
class product_range {
private:
	static constexpr auto p0 = lhs_min * rhs_min;
	static constexpr auto p1 = lhs_min * rhs_max;
	static constexpr auto p2 = lhs_max * rhs_min;
	static constexpr auto p3 = lhs_max * rhs_max;
public:
	static constexpr auto min_product = min(p0, p1, p2, p3);
	static constexpr auto max_product = max(p0, p1, p2, p3);
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
	using common = common_type_t<result_type, ranged_integer<lhs_min, lhs_max, lhs_overflow_policy>, ranged_integer<rhs_min, rhs_max, rhs_overflow_policy>>;
	return result_type(static_cast<common>(lhs).value() * static_cast<common>(rhs).value(), non_check);
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

// Multiplication with built-ins
template<
	intmax_t lhs_min, intmax_t lhs_max,
	typename integer,
	template<intmax_t, intmax_t> class overflow_policy,
	enable_if_t<std::is_integral<integer>::value>...
>
constexpr auto operator*(
	ranged_integer<lhs_min, lhs_max, overflow_policy> const lhs,
	integer const rhs
) noexcept -> decltype(multiply<overflow_policy>(lhs, make_ranged(rhs))) {
	return multiply<overflow_policy>(lhs, make_ranged(rhs));
}
template<
	typename integer,
	intmax_t rhs_min, intmax_t rhs_max,
	template<intmax_t, intmax_t> class overflow_policy,
	enable_if_t<std::is_integral<integer>::value>...
>
constexpr auto operator*(
	integer const lhs,
	ranged_integer<rhs_min, rhs_max, overflow_policy> const rhs
) noexcept -> decltype(multiply<overflow_policy>(make_ranged(lhs), rhs)) {
	return multiply<overflow_policy>(make_ranged(lhs), rhs);
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
	// minimum) so that the 'positive' divisor is not selected in a later step.
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
	static constexpr auto min_quotient = min(l0, l1, l2, l3, g0, g1, g2, g3);
	static constexpr auto max_quotient = max(g0, g1, g2, g3);
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
	using common = common_type_t<result_type, ranged_integer<lhs_min, lhs_max, lhs_overflow_policy>, ranged_integer<rhs_min, rhs_max, rhs_overflow_policy>>;
	return result_type(static_cast<common>(lhs).value() / static_cast<common>(rhs).value(), non_check);
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

// Division with built-ins
template<
	intmax_t lhs_min, intmax_t lhs_max,
	typename integer,
	template<intmax_t, intmax_t> class overflow_policy,
	enable_if_t<std::is_integral<integer>::value>...
>
constexpr auto operator/(
	ranged_integer<lhs_min, lhs_max, overflow_policy> const lhs,
	integer const rhs
) noexcept -> decltype(divide<overflow_policy>(lhs, make_ranged(rhs))) {
	return divide<overflow_policy>(lhs, make_ranged(rhs));
}
template<
	typename integer,
	intmax_t rhs_min, intmax_t rhs_max,
	template<intmax_t, intmax_t> class overflow_policy,
	enable_if_t<std::is_integral<integer>::value>...
>
constexpr auto operator/(
	integer const lhs,
	ranged_integer<rhs_min, rhs_max, overflow_policy> const rhs
) noexcept -> decltype(divide<overflow_policy>(make_ranged(lhs), rhs)) {
	return divide<overflow_policy>(make_ranged(lhs), rhs);
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



// Unary plus

template<intmax_t minimum, intmax_t maximum, template<intmax_t, intmax_t> class OverflowPolicy>
constexpr ranged_integer<minimum, maximum, OverflowPolicy> operator+(ranged_integer<minimum, maximum, OverflowPolicy> const & value) noexcept {
	return value;
}

#endif	// RANGED_INTEGER_ARITHMETIC_OPERATORS_HPP_
