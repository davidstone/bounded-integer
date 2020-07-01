// Copyright David Stone 2019.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <cstdint>

namespace bounded {
namespace detail {

#if defined __GNUC__
	#define BOUNDED_DETAIL_HAS_128_BIT 1
	using int128_t = __int128_t;
	using uint128_t = __uint128_t;
#endif

}	// namespace detail
}	// namespace bounded
