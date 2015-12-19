// Copyright David Stone 2015.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <cstdint>
#include <type_traits>
#include <boost/integer.hpp>

namespace bounded {
namespace detail {

template<typename T1, typename T2>
using larger_type = std::conditional_t<sizeof(T1) >= sizeof(T2), T1, T2>;

#define BOUNDED_INTEGER_UNDERLYING_TYPE(version) \
template<intmax_t minimum, intmax_t maximum> \
using signed_ ## version ## _t = larger_type< \
	typename boost::int_min_value_t<minimum>::version, \
	typename boost::int_max_value_t<maximum>::version \
>; \
\
template<intmax_t minimum, intmax_t maximum> \
using unsigned_ ## version ## _t = typename boost::uint_value_t<static_cast<unsigned long long>(maximum)>::version; \
\
template<intmax_t minimum, intmax_t maximum> \
using version ## _t = std::conditional_t< \
	minimum < 0, \
	signed_ ## version ## _t<minimum, maximum>, \
	unsigned_ ## version ## _t<minimum, maximum> \
>;


BOUNDED_INTEGER_UNDERLYING_TYPE(fast)
BOUNDED_INTEGER_UNDERLYING_TYPE(least)



}	// namespace detail
}	// namespace bounded

