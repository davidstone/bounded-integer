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
#include <boost/integer.hpp>
#include "policy/throw_policy.hpp"

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

template<typename T>
constexpr bool is_implicitly_constructible_from(intmax_t const minimum, intmax_t const maximum) noexcept {
	return type_fits_in_range<typename std::decay<T>::type>(minimum, maximum);
}
template<typename policy, typename T>
constexpr bool is_explicitly_constructible_from(intmax_t const minimum, intmax_t const maximum) noexcept {
	return !is_implicitly_constructible_from<T>(minimum, maximum) and
		(type_overlaps_range<typename std::decay<T>::type>(minimum, maximum) or !policy::overflow_is_error);
}

}	// namespace detail

template<intmax_t minimum, intmax_t maximum, typename OverflowPolicy>
class bounded_integer {
public:
	static_assert(minimum <= maximum, "Maximum cannot be less than minimum");
	using underlying_type = detail::underlying_t<minimum, maximum>;
	using overflow_policy_type = OverflowPolicy;
	static_assert(detail::value_fits_in_type<underlying_type>(minimum), "minimum does not fit in underlying_type.");
	static_assert(detail::value_fits_in_type<underlying_type>(maximum), "maximum does not fit in underlying_type.");

	static_assert(minimum < 0 ? std::numeric_limits<underlying_type>::is_signed : true, "underlying_type should be signed.");

	constexpr bounded_integer() noexcept = default;
	constexpr bounded_integer(bounded_integer const &) noexcept = default;
	constexpr bounded_integer(bounded_integer &&) noexcept = default;
	// The constructor template below should cover the volatile cases

	// Use this constructor if you know by means that cannot be determined by
	// the type system that the value really does fit in the range.
	template<typename integer, enable_if_t<
		detail::is_implicitly_constructible_from<integer>(minimum, maximum) or
		detail::is_explicitly_constructible_from<overflow_policy_type, integer>(minimum, maximum)
	> = clang_dummy>
	constexpr bounded_integer(integer && other, non_check_t) noexcept:
		m_value(static_cast<underlying_type>(std::forward<integer>(other))) {
	}

	// Intentionally implicit: this is safe because the value is in range
	template<typename integer, enable_if_t<
		detail::is_implicitly_constructible_from<integer>(minimum, maximum)
	> = clang_dummy>
	constexpr bounded_integer(integer && other) noexcept:
		bounded_integer(std::forward<integer>(other), non_check) {
	}

	template<typename integer, enable_if_t<
		detail::is_explicitly_constructible_from<overflow_policy_type, integer>(minimum, maximum)
	> = clang_dummy>
	constexpr explicit bounded_integer(integer && other)
		noexcept(noexcept(
			overflow_policy_type{}.assignment(std::forward<integer>(other), minimum, maximum)
		)):
		bounded_integer(overflow_policy_type{}.assignment(std::forward<integer>(other), minimum, maximum), non_check) {
	}

	template<typename integer, enable_if_t<std::is_enum<integer>::value> = clang_dummy>
	constexpr explicit bounded_integer(integer other) noexcept(noexcept(bounded_integer(static_cast<typename std::underlying_type<integer>::type>(other)))):
		bounded_integer(static_cast<typename std::underlying_type<integer>::type>(other)) {
	}

	bounded_integer & operator=(bounded_integer const &) noexcept = default;
	bounded_integer & operator=(bounded_integer &&) noexcept = default;
	// Generate an assignment operator for any value that we have a constructor
	// generated for. I am assuming that these fairly simple templates and
	// copies will all be removed / inlined by the compiler, so this should be
	// no worse than a hand-generated solution. I may be wrong with volatile
	// variables, though. This may also interfere with type_traits
	template<typename integer>
	bounded_integer & operator=(integer && other)
		noexcept(std::is_nothrow_constructible<bounded_integer, integer &&>::value) {
		*this = bounded_integer(std::forward<integer>(other));
		return *this;
	}
	template<typename integer>
	bounded_integer volatile & operator=(integer && other) volatile
		noexcept(std::is_nothrow_constructible<bounded_integer, integer &&>::value) {
		*this = bounded_integer(std::forward<integer>(other));
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

}	// namespace bounded_integer

#endif	// BOUNDED_INTEGER_CLASS_HPP_
