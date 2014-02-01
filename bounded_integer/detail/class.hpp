// bounded_integer type main implementation
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

#ifndef BOUNDED_INTEGER_CLASS_HPP_
#define BOUNDED_INTEGER_CLASS_HPP_

#include "enable_if.hpp"
#include "forward_declaration.hpp"
#include "overlapping_range.hpp"

#include <cstdint>
#include <limits>
#include <type_traits>
#include <utility>
#include <boost/integer.hpp>

namespace bounded_integer {
namespace detail {

template<typename T1, typename T2>
using larger_type = typename std::conditional<sizeof(T1) >= sizeof(T2), T1, T2>::type;

template<intmax_t minimum, intmax_t maximum>
using signed_underlying_t = larger_type<typename boost::int_min_value_t<minimum>::least, typename boost::int_max_value_t<maximum>::least>;

template<intmax_t minimum, intmax_t maximum>
using unsigned_underlying_t = typename boost::uint_value_t<static_cast<unsigned long long>(maximum)>::least;

template<intmax_t minimum, intmax_t maximum>
using underlying_t = typename std::conditional<
	minimum < 0,
	signed_underlying_t<minimum, maximum>,
	unsigned_underlying_t<minimum, maximum>
>::type;

template<typename T, enable_if_t<std::numeric_limits<typename std::decay<T>::type>::is_specialized> = clang_dummy>
constexpr bool is_implicitly_constructible_from(intmax_t const minimum, intmax_t const maximum) noexcept {
	return type_fits_in_range<typename std::decay<T>::type>(minimum, maximum);
}
template<typename T, enable_if_t<!std::numeric_limits<typename std::decay<T>::type>::is_specialized> = clang_dummy>
constexpr bool is_implicitly_constructible_from(intmax_t, intmax_t) noexcept {
	return false;
}

template<typename policy, typename T, enable_if_t<std::numeric_limits<typename std::decay<T>::type>::is_specialized> = clang_dummy>
constexpr bool is_explicitly_constructible_from(intmax_t const minimum, intmax_t const maximum) noexcept {
	return type_overlaps_range<typename std::decay<T>::type>(minimum, maximum) or !policy::overflow_is_error;
}
template<typename policy, typename T, enable_if_t<!std::numeric_limits<typename std::decay<T>::type>::is_specialized> = clang_dummy>
constexpr bool is_explicitly_constructible_from(intmax_t, intmax_t) noexcept {
	return false;
}

template<intmax_t minimum, intmax_t maximum, typename OverflowPolicy>
class base {
public:
	static_assert(minimum <= maximum, "Maximum cannot be less than minimum");
	using underlying_type = underlying_t<minimum, maximum>;
	using overflow_policy_type = OverflowPolicy;
	static_assert(value_fits_in_type<underlying_type>(minimum), "minimum does not fit in underlying_type.");
	static_assert(value_fits_in_type<underlying_type>(maximum), "maximum does not fit in underlying_type.");

	static_assert(minimum < 0 ? std::numeric_limits<underlying_type>::is_signed : true, "underlying_type should be signed.");

	constexpr base() noexcept = default;
	constexpr base(base const &) noexcept = default;
	constexpr base(base &&) noexcept = default;
	constexpr base(base const volatile & other) noexcept:
		m_value(other.m_value) {
	}
	constexpr base(base volatile && other) noexcept:
		m_value(std::move(other.m_value)) {
	}

	// Use this constructor if you know by means that cannot be determined by
	// the type system that the value really does fit in the range.
	template<typename integer, enable_if_t<
		is_explicitly_constructible_from<overflow_policy_type, integer>(minimum, maximum)
	> = clang_dummy>
	constexpr base(integer && other, non_check_t) noexcept:
		m_value(static_cast<underlying_type>(std::forward<integer>(other))) {
	}

	// Intentionally implicit: this is safe because the value is in range
	template<typename integer, enable_if_t<
		is_implicitly_constructible_from<integer>(minimum, maximum)
	> = clang_dummy>
	constexpr base(integer && other) noexcept:
		base(std::forward<integer>(other), non_check) {
	}

	template<typename integer, enable_if_t<
		!is_implicitly_constructible_from<integer>(minimum, maximum) and
		is_explicitly_constructible_from<overflow_policy_type, integer>(minimum, maximum)
	> = clang_dummy>
	constexpr explicit base(integer && other)
		noexcept(noexcept(
			overflow_policy_type{}.assignment(std::forward<integer>(other), minimum, maximum)
		)):
		base(overflow_policy_type{}.assignment(std::forward<integer>(other), minimum, maximum), non_check) {
	}

	template<typename integer, enable_if_t<std::is_enum<integer>::value> = clang_dummy>
	constexpr explicit base(integer other) noexcept(noexcept(base(static_cast<typename std::underlying_type<integer>::type>(other)))):
		base(static_cast<typename std::underlying_type<integer>::type>(other)) {
	}

	base & operator=(base const &) noexcept = default;
	base & operator=(base &&) noexcept = default;
	// Generate an assignment operator for any value that we have a constructor
	// generated for. I am assuming that these fairly simple templates and
	// copies will all be removed / inlined by the compiler, so this should be
	// no worse than a hand-generated solution. I may be wrong with volatile
	// variables, though. This may also interfere with type_traits
	template<typename integer>
	base & operator=(integer && other)
		noexcept(std::is_nothrow_constructible<base, integer &&>::value) {
		*this = base(std::forward<integer>(other));
		return *this;
	}
	template<typename integer>
	base volatile & operator=(integer && other) volatile
		noexcept(std::is_nothrow_constructible<base, integer &&>::value) {
		*this = base(std::forward<integer>(other));
		return *this;
	}
	
	constexpr underlying_type const & value() const noexcept {
		return m_value;
	}
	constexpr underlying_type const volatile & value() const volatile noexcept {
		return m_value;
	}
	// Do not verify that the value is in range because the user has requested a
	// conversion out of the safety of bounded_integer. It is subject to the all
	// the standard rules of conversion from one integer type to another.
	template<typename integer, enable_if_t<std::is_integral<integer>::value> = clang_dummy>
	constexpr explicit operator integer() const noexcept {
		return static_cast<integer>(m_value);
	}
	template<typename integer, enable_if_t<std::is_integral<integer>::value> = clang_dummy>
	constexpr explicit operator integer() const volatile noexcept {
		return static_cast<integer>(m_value);
	}
private:
	underlying_type m_value;
};


}	// namespace detail

#define BOUNDED_INTEGER_COMMON_FUNCTIONALITY \
	using overflow_policy_type = typename base::overflow_policy_type; \
	using underlying_type = typename base::underlying_type; \
	\
	constexpr underlying_type const & value() const noexcept { \
		return m_value.value(); \
	} \
	constexpr underlying_type const volatile & value() const volatile noexcept { \
		return m_value.value(); \
	} \
	template<typename integer, enable_if_t<std::is_integral<integer>::value> = clang_dummy> \
	constexpr explicit operator integer() const noexcept { \
		return static_cast<integer>(m_value); \
	} \
	template<typename integer, enable_if_t<std::is_integral<integer>::value> = clang_dummy> \
	constexpr explicit operator integer() const volatile noexcept { \
		return static_cast<integer>(m_value); \
	}
	
#define BOUNDED_INTEGER_CONSTRUCTORS_AND_ASSIGNMENT_OPERATORS \
	constexpr bounded_integer(bounded_integer const &) noexcept = default; \
	constexpr bounded_integer(bounded_integer &&) noexcept = default; \
	bounded_integer & operator=(bounded_integer const &) noexcept = default; \
	bounded_integer & operator=(bounded_integer &&) noexcept = default; \
	\
	template<typename T, enable_if_t<!std::is_convertible<T &&, base>::value and std::is_constructible<base, T &&>::value> = clang_dummy> \
	explicit constexpr bounded_integer(T && other) noexcept(std::is_nothrow_constructible<base, T &&>::value): \
		bounded_integer(overflow_policy_type{}.assignment(std::forward<T>(other), minimum, maximum), non_check) { \
	} \
	\
	template<typename integer> \
	bounded_integer & operator=(integer && other) noexcept(noexcept( \
		m_value = base( \
			overflow_policy_type{}.assignment( \
				std::forward<integer>(other), \
				static_cast<intmax_t>(std::declval<min_type>()), \
				static_cast<intmax_t>(std::declval<max_type>()) \
			), \
			non_check \
		) \
	)) { \
		m_value = base( \
			overflow_policy_type{}.assignment( \
				std::forward<integer>(other), \
				static_cast<intmax_t>(min()), \
				static_cast<intmax_t>(max()) \
			), \
			non_check \
		); \
		return *this; \
	} \
	template<typename integer> \
	bounded_integer volatile & operator=(integer && other) volatile noexcept(noexcept( \
	m_value = base( \
			overflow_policy_type{}.assignment( \
				std::forward<integer>(other), \
				static_cast<intmax_t>(std::declval<min_type>()), \
				static_cast<intmax_t>(std::declval<max_type>()) \
			), \
			non_check \
		) \
	)) { \
		m_value = base( \
			overflow_policy_type{}.assignment( \
				std::forward<integer>(other), \
				static_cast<intmax_t>(min()), \
				static_cast<intmax_t>(max()) \
			), \
			non_check \
		); \
		return *this; \
	}


template<intmax_t minimum, intmax_t maximum, typename OverflowPolicy>
class bounded_integer<minimum, maximum, OverflowPolicy, bounds::static_min_max> {
private:
	using base = detail::base<minimum, maximum, OverflowPolicy>;
	base m_value;
public:
	BOUNDED_INTEGER_COMMON_FUNCTIONALITY
	
	constexpr bounded_integer() noexcept = default;

	// m_value's constructor handles static range checking
	template<typename integer, enable_if_t<std::is_convertible<integer &&, base>::value> = clang_dummy>
	constexpr bounded_integer(integer && other) noexcept:
		m_value(std::forward<integer>(other), non_check) {
	}
	template<typename T, enable_if_t<std::is_constructible<base, T &&, non_check_t>::value> = clang_dummy>
	explicit constexpr bounded_integer(T && other, non_check_t) noexcept:
		m_value(std::forward<T>(other), non_check) {
	}
	
	
	using min_type = bounded_integer<minimum, minimum, overflow_policy_type>;
	using max_type = bounded_integer<maximum, maximum, overflow_policy_type>;
	static constexpr min_type min() noexcept {
		return min_type(minimum, non_check);
	}
	static constexpr max_type max() noexcept {
		return max_type(maximum, non_check);
	}
	
	BOUNDED_INTEGER_CONSTRUCTORS_AND_ASSIGNMENT_OPERATORS
};





// TODO: determine what should happen if a bounded_integer with a dynamic min
// and max is constructed from a class with just a dynamic min or max.
template<intmax_t minimum, intmax_t maximum, typename OverflowPolicy>
class bounded_integer<minimum, maximum, OverflowPolicy, bounds::dynamic_min> {
private:
	using base = detail::base<minimum, maximum, OverflowPolicy>;
	base m_min;
	base m_value;
public:
	BOUNDED_INTEGER_COMMON_FUNCTIONALITY

	constexpr bounded_integer() noexcept:
		m_min(minimum, non_check),
		m_value{} {
	}

	constexpr bounded_integer(bounded_integer const volatile & other) noexcept:
		m_min(other.m_min),
		m_value(other.m_value) {
	}
	constexpr bounded_integer(bounded_integer volatile && other) noexcept:
		m_min(std::move(other.m_min)),
		m_value(std::move(other.m_value)) {
	}

	template<typename T, enable_if_t<std::is_convertible<T &&, base>::value> = clang_dummy>
	constexpr bounded_integer(T && other_value) noexcept:
		m_min(minimum, non_check),
		m_value(std::forward<T>(other_value)) {
	}
	
	template<typename T, enable_if_t<std::is_constructible<base, T &&, non_check_t>::value> = clang_dummy>
	explicit constexpr bounded_integer(T && other, non_check_t) noexcept:
		bounded_integer(std::forward<T>(other), minimum, non_check) {
	}
	
	template<typename T, typename Min>
	constexpr bounded_integer(T && other_value, Min && other_min, non_check_t) noexcept:
		m_min(std::forward<Min>(other_min), non_check),
		m_value(std::forward<T>(other_value), non_check) {
	}

	template<typename T, typename Min>
	constexpr bounded_integer(T && other_value, Min && other_min)
		noexcept(noexcept(
			overflow_policy_type{}.assignment(
				std::forward<Min>(other_min),
				minimum,
				maximum
			) and
			overflow_policy_type{}.assignment(
				std::forward<T>(other_value),
				static_cast<intmax_t>(other_min),
				maximum
			)
		)):
		m_min(overflow_policy_type{}.assignment(
			std::forward<Min>(other_min),
			minimum,
			maximum
		)),
		m_value(overflow_policy_type{}.assignment(
			std::forward<T>(other_value),
			static_cast<intmax_t>(other_min),
			maximum
		)) {
	}

	using min_type = bounded_integer<minimum, maximum, overflow_policy_type>;
	using max_type = bounded_integer<maximum, maximum, overflow_policy_type>;
	constexpr min_type min() const noexcept {
		return min_type(m_min.value(), non_check);
	}
	static constexpr max_type max() noexcept {
		return max_type(maximum, non_check);
	}

	BOUNDED_INTEGER_CONSTRUCTORS_AND_ASSIGNMENT_OPERATORS
};





template<intmax_t minimum, intmax_t maximum, typename OverflowPolicy>
class bounded_integer<minimum, maximum, OverflowPolicy, bounds::dynamic_max> {
private:
	using base = detail::base<minimum, maximum, OverflowPolicy>;
	base m_max;
	base m_value;
public:
	BOUNDED_INTEGER_COMMON_FUNCTIONALITY

	constexpr bounded_integer() noexcept:
		m_max(maximum, non_check),
		m_value{} {
	}

	constexpr bounded_integer(bounded_integer const volatile & other) noexcept:
		m_max(other.m_max),
		m_value(other.m_value) {
	}
	constexpr bounded_integer(bounded_integer volatile && other) noexcept:
		m_max(std::move(other.m_max)),
		m_value(std::move(other.m_value)) {
	}

	template<typename T, enable_if_t<std::is_convertible<T &&, base>::value> = clang_dummy>
	constexpr bounded_integer(T && other_value) noexcept:
		m_max(maximum, non_check),
		m_value(std::forward<T>(other_value)) {
	}
	
	template<typename T, enable_if_t<std::is_constructible<base, T &&, non_check_t>::value> = clang_dummy>
	explicit constexpr bounded_integer(T && other, non_check_t) noexcept:
		bounded_integer(std::forward<T>(other), maximum, non_check) {
	}
	
	

	template<typename T, typename Max>
	constexpr bounded_integer(T && other_value, Max && other_max, non_check_t) noexcept:
		m_max(std::forward<Max>(other_max), non_check),
		m_value(std::forward<T>(other_value), non_check) {
	}

	template<typename T, typename Max>
	constexpr bounded_integer(T && other_value, Max && other_max)
		noexcept(noexcept(
			overflow_policy_type{}.assignment(
				std::forward<Max>(other_max),
				minimum,
				maximum
			) and
			overflow_policy_type{}.assignment(
				std::forward<T>(other_value),
				minimum,
				static_cast<intmax_t>(other_max)
			)
		)):
		m_max(overflow_policy_type{}.assignment(
			std::forward<Max>(other_max),
			minimum,
			maximum
		)),
		m_value(overflow_policy_type{}.assignment(
			std::forward<T>(other_value),
			minimum,
			static_cast<intmax_t>(other_max)
		)) {
	}
	
	using min_type = bounded_integer<minimum, minimum, overflow_policy_type>;
	using max_type = bounded_integer<minimum, maximum, overflow_policy_type>;
	static constexpr min_type min() noexcept {
		return min_type(minimum, non_check);
	}
	constexpr max_type max() const noexcept {
		return max_type(m_max.value(), non_check);
	}

	BOUNDED_INTEGER_CONSTRUCTORS_AND_ASSIGNMENT_OPERATORS
};





template<intmax_t minimum, intmax_t maximum, typename OverflowPolicy>
class bounded_integer<minimum, maximum, OverflowPolicy, bounds::dynamic_min_max> {
private:
	using base = detail::base<minimum, maximum, OverflowPolicy>;
	base m_min;
	base m_max;
	base m_value;
public:
	BOUNDED_INTEGER_COMMON_FUNCTIONALITY

	constexpr bounded_integer() noexcept:
		m_min(minimum, non_check),
		m_max(maximum, non_check),
		m_value{} {
	}

	constexpr bounded_integer(bounded_integer const volatile & other) noexcept:
		m_min(other.m_min),
		m_max(other.m_max),
		m_value(other.m_value) {
	}
	constexpr bounded_integer(bounded_integer volatile && other) noexcept:
		m_min(std::move(other.m_min)),
		m_max(std::move(other.m_max)),
		m_value(std::move(other.m_value)) {
	}

	template<typename T, enable_if_t<std::is_convertible<T &&, base>::value> = clang_dummy>
	constexpr bounded_integer(T && other_value) noexcept:
		m_min(minimum, non_check),
		m_max(maximum, non_check),
		m_value(std::forward<T>(other_value)) {
	}
	
	template<typename T, enable_if_t<std::is_constructible<base, T &&, non_check_t>::value> = clang_dummy>
	explicit constexpr bounded_integer(T && other, non_check_t) noexcept:
		bounded_integer(std::forward<T>(other), minimum, maximum, non_check) {
	}
	
	template<typename T, typename Min, typename Max>
	constexpr bounded_integer(T && other_value, Min && other_min, Max && other_max, non_check_t) noexcept:
		m_min(std::forward<Min>(other_min), non_check),
		m_max(std::forward<Max>(other_max), non_check),
		m_value(std::forward<T>(other_value), non_check) {
	}

	// TODO: gcc complains about "sorry, unimplemented: use of the value of the
	// object being constructed in a constant expression" when I try to minimize
	// calls to the overflow policy, so I have to always operate on the
	// parameters.
	
	// TODO: determine what should happen if max < min
	template<typename T, typename Min, typename Max>
	constexpr bounded_integer(T && other_value, Min && other_min, Max && other_max)
		noexcept(noexcept(
			overflow_policy_type{}.assignment(
				std::forward<Min>(other_min),
				minimum,
				(other_max < maximum ? static_cast<intmax_t>(other_max) : maximum)
			) and
			overflow_policy_type{}.assignment(
				std::forward<Max>(other_max),
				static_cast<intmax_t>(other_min),
				maximum
			) and
			overflow_policy_type{}.assignment(
				std::forward<T>(other_value),
				static_cast<intmax_t>(other_min),
				static_cast<intmax_t>(other_max)
			)
		)):
		m_min(overflow_policy_type{}.assignment(
			std::forward<Min>(other_min),
			minimum,
			(other_max < maximum ? static_cast<intmax_t>(other_max) : maximum)
		)),
		m_max(overflow_policy_type{}.assignment(
			std::forward<Max>(other_max),
			static_cast<intmax_t>(other_min),
			maximum
		)),
		m_value(overflow_policy_type{}.assignment(
			std::forward<T>(other_value),
			static_cast<intmax_t>(other_min),
			static_cast<intmax_t>(other_max)
		)) {
	}
	
	using min_type = bounded_integer<minimum, maximum, overflow_policy_type>;
	using max_type = bounded_integer<minimum, maximum, overflow_policy_type>;
	constexpr min_type min() const noexcept {
		return min_type(m_min.value(), non_check);
	}
	constexpr max_type max() const noexcept {
		return max_type(m_max.value(), non_check);
	}

	BOUNDED_INTEGER_CONSTRUCTORS_AND_ASSIGNMENT_OPERATORS
};

#undef BOUNDED_INTEGER_ASSIGNMENT_OPERATORS
#undef BOUNDED_INTEGER_COMMON_FUNCTIONALITY

}	// namespace bounded_integer

#endif	// BOUNDED_INTEGER_CLASS_HPP_
