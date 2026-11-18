// Copyright David Stone 2017.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

export module bounded.arithmetic.pointer;

import bounded.bounded_integer;

namespace bounded {

export template<typename T>
constexpr auto operator+(T * const & ptr, bounded_integer auto const number) -> T * {
	return ptr + number.value();
}


export template<typename T>
constexpr auto operator+(bounded_integer auto const number, T * const & ptr) -> T * {
	return number.value() + ptr;
}

} // namespace bounded