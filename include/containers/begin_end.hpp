// Copyright David Stone 2016.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/algorithms/move_iterator.hpp>

#include <operators/forward.hpp>
#include <bounded/integer.hpp>

#include <iterator>
#include <utility>

namespace containers {
namespace detail {
namespace common {

template<typename Return, typename T>
void rvalue_ref_qualified(Return (T::*)() &&);

#define CONTAINERS_DETAIL_BEGIN_END_OVERLOADS(begin_or_end) \
\
using std::begin_or_end; \
\
\
template<typename T> \
concept has_rvalue_ ## begin_or_end = requires { rvalue_ref_qualified(&T::begin_or_end); }; \
\
\
template<typename Range> requires( \
	std::is_rvalue_reference_v<Range &&> and \
	has_rvalue_ ## begin_or_end<Range> \
) \
constexpr auto begin_or_end(Range && range) { \
	return std::move(range).begin_or_end(); \
}

CONTAINERS_DETAIL_BEGIN_END_OVERLOADS(begin)
CONTAINERS_DETAIL_BEGIN_END_OVERLOADS(end)



#define CONTAINERS_COMMON_BEGIN_END_USING_DECLARATIONS \
	using ::containers::detail::common::begin; \
	using ::containers::detail::common::end;

}	// namespace common

CONTAINERS_COMMON_BEGIN_END_USING_DECLARATIONS

}	// namespace detail

CONTAINERS_COMMON_BEGIN_END_USING_DECLARATIONS

}	// namespace containers
