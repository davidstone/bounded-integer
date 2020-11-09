// Copyright David Stone 2017.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <bounded/detail/is_bounded_integer.hpp>
#include <bounded/check_in_range.hpp>

#include <iosfwd>
#include <type_traits>

namespace bounded {

template<typename CharT, typename Traits>
decltype(auto) operator<<(std::basic_ostream<CharT, Traits> & out, bounded_integer auto const x) {
	// The unary plus applies integer promotions to x. This ensures values are
	// printed as integers. Without this, I could run into an issue where the
	// underlying type is a typedef for char / signed char / unsigned char.
	// This would output the value as though it were a character.
	return out << +x.value();
}

template<typename CharT, typename Traits, bounded_integer Integer>
decltype(auto) operator>>(std::basic_istream<CharT, Traits> & in, Integer & x) {
	using underlying = typename Integer::underlying_type;
	constexpr auto use_int = sizeof(underlying) < sizeof(int) or (sizeof(underlying) == sizeof(int) and std::is_signed_v<underlying>);
	constexpr auto use_unsigned = sizeof(underlying) == sizeof(unsigned) and std::is_unsigned_v<underlying>;
	using intermediate =
		std::conditional_t<use_int, int,
		std::conditional_t<use_unsigned, unsigned,
		underlying
	>>;
	intermediate temp;
	in >> temp;
	x = check_in_range<Integer>(integer(temp));
	return in;
}

} // namespace bounded
