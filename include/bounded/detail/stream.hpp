// Copyright David Stone 2015.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <bounded/detail/is_bounded_integer.hpp>
#include <bounded/detail/requires.hpp>

#include <cstdint>
#include <istream>
#include <ostream>

namespace bounded {

template<typename CharT, typename Traits, typename Integer, BOUNDED_REQUIRES(is_bounded_integer<Integer>)>
decltype(auto) operator<<(std::basic_ostream<CharT, Traits> & out, Integer const & x) {
	// The unary plus applies integer promotions to x. This ensures values are
	// printed as integers. Without this, I could run into an issue where the
	// underlying type is a typedef for char / signed char / unsigned char.
	// This would output the value as though it were a character.
	return out << +x.value();
}
template<typename CharT, typename Traits, typename Integer, BOUNDED_REQUIRES(is_bounded_integer<Integer>)>
decltype(auto) operator<<(std::basic_ostream<CharT, Traits> & out, Integer const volatile & x) {
	return out << +x.value();
}

template<typename CharT, typename Traits, typename Integer, BOUNDED_REQUIRES(is_bounded_integer<Integer>)>
decltype(auto) operator>>(std::basic_istream<CharT, Traits> & in, Integer & x) {
	typename Integer::underlying_type temp;
	in >> temp;
	x = temp;
	return in;
}
template<typename CharT, typename Traits, typename Integer, BOUNDED_REQUIRES(is_bounded_integer<Integer>)>
decltype(auto) operator>>(std::basic_istream<CharT, Traits> & in, Integer volatile & x) {
	typename Integer::underlying_type temp;
	in >> temp;
	x = temp;
	return in;
}

}	// namespace bounded
