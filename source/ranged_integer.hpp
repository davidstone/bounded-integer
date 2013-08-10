// Ranged integer type
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

#ifndef RANGED_INTEGER_HPP_
#define RANGED_INTEGER_HPP_

#include <cstdint>
#include <limits>
#include <stdexcept>
#include <type_traits>
#include <iostream>
#include <boost/integer.hpp>

#include "enable_if.hpp"

enum class signed_arithmetic {
	any, force_signed, force_unsigned
};

namespace detail {

template<typename T1, typename T2>
using larger_type = typename std::conditional<sizeof(T1) >= sizeof(T2), T1, T2>::type;

template<intmax_t minimum, intmax_t maximum>
using signed_underlying_t = larger_type<typename boost::int_min_value_t<minimum>::least, typename boost::int_max_value_t<maximum>::least>;

template<uintmax_t minimum, uintmax_t maximum>
using unsigned_underlying_t = typename boost::uint_value_t<maximum>::least;

template<intmax_t minimum, intmax_t maximum, bool must_be_signed>
using underlying_t = typename std::conditional<
	minimum < 0 or must_be_signed,
	signed_underlying_t<minimum, maximum>,
	unsigned_underlying_t<minimum, maximum>
>::type;

}	// namespace detail

class throw_on_overflow {
public:
	template<intmax_t minimum, intmax_t maximum, typename integer>
	static constexpr integer enforce(integer const new_value) {
		return (new_value < minimum) ?
			throw std::underflow_error{"Value too small"} :
			((new_value > maximum) ?
				throw std::overflow_error{"Value too large"} :
				new_value);
	}
};

template<intmax_t minimum, intmax_t maximum, signed_arithmetic s, typename OverflowPolicy>
class ranged_integer {
public:
	using underlying_type = detail::underlying_t<minimum, maximum, s == signed_arithmetic::force_signed>;
	constexpr explicit ranged_integer(underlying_type value):
		m_value(OverflowPolicy::template enforce<minimum, maximum>(value)) {
	}
	constexpr ranged_integer(ranged_integer const & other) noexcept = default;
	constexpr ranged_integer(ranged_integer && other) noexcept = default;
	ranged_integer & operator=(ranged_integer const & other) noexcept = default;
	ranged_integer & operator=(ranged_integer && other) noexcept = default;

	// No checks if we are constructing from a ranged_integer that fits entirely
	// within the range of this ranged_integer.
	template<intmax_t other_min, intmax_t other_max, signed_arithmetic other_arithmetic, typename other_overflow_policy, enable_if_t<
			other_min >= minimum and other_max <= maximum
	>...>
	constexpr ranged_integer(ranged_integer<other_min, other_max, other_arithmetic, other_overflow_policy> const & other) noexcept:
		m_value(other.m_value) {
	}

	// Allow an explicit conversion from one ranged_integer type to another as
	// long as the values at least have some overlap
	template<intmax_t other_min, intmax_t other_max, signed_arithmetic other_arithmetic, typename other_overflow_policy, enable_if_t<
		((other_min < minimum) or (other_max > maximum))
		and (other_min <= maximum and other_max >= minimum)
	>...>
	constexpr explicit ranged_integer(ranged_integer<other_min, other_max, other_arithmetic, other_overflow_policy> const & other):
		ranged_integer(static_cast<underlying_type>(other.m_value)) {
	}

	ranged_integer & operator=(underlying_type value) {
		m_value = OverflowPolicy::template enforce<minimum, maximum>(value);
		return *this;
	}
	constexpr operator underlying_type() const noexcept {
		return m_value;
	}
private:
	template<intmax_t other_min, intmax_t other_max, signed_arithmetic other_arithmetic, typename other_overflow_policy>
	friend class ranged_integer;
	underlying_type m_value;
};

template<intmax_t minimum, intmax_t maximum>
using checked_integer = ranged_integer<minimum, maximum, signed_arithmetic::any, throw_on_overflow>;

#endif	// RANGED_INTEGER_HPP_
