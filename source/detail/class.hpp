// Ranged integer type main implementation
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

#ifndef RANGED_INTEGER_CLASS_HPP_
#define RANGED_INTEGER_CLASS_HPP_

#include <cstdint>
#include <limits>
#include <type_traits>
#include <boost/integer.hpp>

#include "enable_if.hpp"

namespace detail {

template<typename T1, typename T2>
using larger_type = typename std::conditional<sizeof(T1) >= sizeof(T2), T1, T2>::type;

template<intmax_t minimum, intmax_t maximum>
using signed_underlying_t = larger_type<typename boost::int_min_value_t<minimum>::least, typename boost::int_max_value_t<maximum>::least>;

template<uintmax_t minimum, uintmax_t maximum>
using unsigned_underlying_t = typename boost::uint_value_t<maximum>::least;

template<intmax_t minimum, intmax_t maximum>
using underlying_t = typename std::conditional<
	minimum < 0,
	signed_underlying_t<minimum, maximum>,
	unsigned_underlying_t<minimum, maximum>
>::type;

}	// namespace detail

// Does not verify that the value is in range with the policy
enum non_check_t { non_check };

template<intmax_t minimum, intmax_t maximum, template<intmax_t, intmax_t> class OverflowPolicy>
class ranged_integer {
public:
	using underlying_type = detail::underlying_t<minimum, maximum>;
	using overflow_policy = OverflowPolicy<minimum, maximum>;
	static constexpr intmax_t min = minimum;
	static constexpr intmax_t max = maximum;
	constexpr explicit ranged_integer(underlying_type other):
		m_value(overflow_policy{}(other)) {
	}

	constexpr ranged_integer(underlying_type const other, non_check_t) noexcept:
		m_value(other) {
	}

	constexpr ranged_integer(ranged_integer const & other) noexcept = default;
	constexpr ranged_integer(ranged_integer && other) noexcept = default;
	ranged_integer & operator=(ranged_integer const & other) noexcept = default;
	ranged_integer & operator=(ranged_integer && other) noexcept = default;

	// No checks if we are constructing from a ranged_integer that fits entirely
	// within the range of this ranged_integer.
	template<intmax_t other_min, intmax_t other_max, template<intmax_t, intmax_t> class other_overflow_policy, enable_if_t<
			other_min >= minimum and other_max <= maximum
	>...>
	constexpr ranged_integer(ranged_integer<other_min, other_max, other_overflow_policy> const & other) noexcept:
		m_value(other.m_value) {
	}

	// Allow an explicit conversion from one ranged_integer type to another as
	// long as the values at least have some overlap
	template<intmax_t other_min, intmax_t other_max, template<intmax_t, intmax_t> class other_overflow_policy, enable_if_t<
		((other_min < minimum) or (other_max > maximum))
		and (other_min <= maximum and other_max >= minimum)
	>...>
	constexpr explicit ranged_integer(ranged_integer<other_min, other_max, other_overflow_policy> const & other):
		ranged_integer(static_cast<underlying_type>(other.m_value)) {
	}

	ranged_integer & operator=(underlying_type const other) {
		m_value = overflow_policy{}(other);
		return *this;
	}

	constexpr underlying_type value() const noexcept {
		return m_value;
	}
	constexpr operator underlying_type() const noexcept {
		return value();
	}
private:
	template<intmax_t other_min, intmax_t other_max, template<intmax_t, intmax_t> class other_overflow_policy>
	friend class ranged_integer;
	underlying_type m_value;
};

#endif	// RANGED_INTEGER_CLASS_HPP_
