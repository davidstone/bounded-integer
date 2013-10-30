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

#include "enable_if.hpp"
#include "overlapping_range.hpp"

#include <cstdint>
#include <limits>
#include <type_traits>
#include <boost/integer.hpp>

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

template<intmax_t minimum, intmax_t maximum>
class ranged_integer_base {
public:
	using underlying_type = detail::underlying_t<minimum, maximum>;
	constexpr ranged_integer_base(underlying_type const other) noexcept:
		m_value(std::move(other)) {
	}
	constexpr underlying_type value() const noexcept {
		return m_value;
	}
private:
	underlying_type m_value;
};

template<intmax_t only_value>
class ranged_integer_base<only_value, only_value> {
public:
	using underlying_type = detail::underlying_t<only_value, only_value>;
	constexpr ranged_integer_base() noexcept = default;
	constexpr ranged_integer_base(underlying_type) noexcept {
	}
	constexpr underlying_type value() const noexcept {
		return only_value;
	}
};

}	// namespace detail

// I use private inheritance here to take advantage of the empty base
// optimization when minimum == maximum
template<intmax_t minimum, intmax_t maximum, template<intmax_t, intmax_t> class OverflowPolicy>
class ranged_integer : private detail::ranged_integer_base<minimum, maximum> {
private:
	using base = detail::ranged_integer_base<minimum, maximum>;
public:
	static_assert(minimum <= maximum, "Maximum cannot be less than minimum");
	using typename base::underlying_type;
	using overflow_policy = OverflowPolicy<minimum, maximum>;
	static_assert(detail::entirely_in_range<underlying_type>(minimum, maximum), "Not all values can fit in the range of the underlying_type.");

	static_assert(minimum < 0 ? std::numeric_limits<underlying_type>::is_signed : true, "Underlying type should be signed.");

	// This is only defined for constant ranges. See ranged_integer_base
	constexpr ranged_integer() noexcept = default;
	constexpr ranged_integer(ranged_integer const & other) noexcept = default;
	constexpr ranged_integer(ranged_integer && other) noexcept = default;
	ranged_integer & operator=(ranged_integer const & other) noexcept = default;
	ranged_integer & operator=(ranged_integer && other) noexcept = default;

	// Use this constructor if you know by means that cannot be determined by
	// the type system that the value really does fit in the range.
	template<typename integer, enable_if_t<
		detail::has_overlap<integer>(minimum, maximum)
	> = clang_dummy>
	constexpr ranged_integer(integer const other, non_check_t) noexcept:
		base(static_cast<underlying_type>(other)) {
	}

	// Intentionally implicit: this is safe because the value is in range
	template<typename integer, enable_if_t<
		detail::type_in_range<integer>(minimum, maximum)
	> = clang_dummy>
	constexpr ranged_integer(integer other) noexcept:
		ranged_integer(other, non_check) {
	}

	template<typename integer, enable_if_t<
		!detail::type_in_range<integer>(minimum, maximum) and
		(detail::has_overlap<integer>(minimum, maximum) or
		!overflow_policy::overflow_is_error)
	> = clang_dummy>
	constexpr explicit ranged_integer(integer other):
		ranged_integer(overflow_policy{}(std::move(other)), non_check) {
	}

	// Generate an assignment operator for any value that we have a constructor
	// generated for. I am assuming that these fairly simple templates and
	// copies will all be removed / inlined by the compiler, so this should be
	// no worse than a hand-generated solution.
	template<typename integer>
	ranged_integer & operator=(integer other) {
		*this = ranged_integer(std::move(other));
		return *this;
	}
	
	using base::value;
	// I do not verify that the value is in range or anything because the user
	// has requested a conversion out of the safety of the ranged_integer type.
	// It is subject to the all of the standard rules that conversion from one
	// built-in integer type to another has.
	template<typename integer, enable_if_t<std::is_integral<integer>::value> = clang_dummy>
	constexpr explicit operator integer() const {
		return static_cast<integer>(value());
	}
};


#endif	// RANGED_INTEGER_CLASS_HPP_
