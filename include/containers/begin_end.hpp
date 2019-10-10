// Copyright David Stone 2016.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/algorithms/move_iterator.hpp>
#include <containers/is_container.hpp>

#include <bounded/detail/forward.hpp>
#include <bounded/integer.hpp>

#include <iterator>
#include <utility>

namespace containers {
namespace detail {
namespace common {

// Provide a free function begin that resolves to member begin if the user has
// defined one with the rvalue qualifier, otherwise default to a definition
// based on the lvalue begin function.
//
// It would be nice if we could define mutable begin from const begin, and at
// first it seems possible to do so at least for containers with contiguous
// iterators. It would look something like (assuming pointers for iterators):
//
// return const_cast<Container::value_type *>(begin(std::as_const(container)));
//
// However, this assumes that a mutable container implies mutable access to the
// elements in the container. This is not necessarily the case, making this
// generic function dangerous for some types. It is better to accept the burden
// of defining essentially the same function (const and mutable begin and end)
// for types that want to allow mutable access to the elements.

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
} \
\
template<container Container> requires( \
	std::is_rvalue_reference_v<Container &&> and \
	!has_rvalue_ ## begin_or_end<std::decay_t<Container>> \
) \
constexpr auto begin_or_end(Container && container) { \
	return ::containers::move_iterator(begin_or_end(container)); \
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
