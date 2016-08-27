// Copyright David Stone 2016.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/algorithms/move_iterator.hpp>
#include <containers/algorithms/reverse_iterator.hpp>
#include <containers/is_container.hpp>

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
// I would like to define containers::begin and containers::end to forward to
// member begin and end if it is defined, but then unqualified calls to begin
// and end become ambiguous with std::begin and std::end. I would be happy with
// std::begin and std::end except they do not have a noexcept specifier.
//
// It would be nice if we could define mutable begin from const begin, and at
// first it seems possible to do so at least for containers with contiguous
// iterators. It would look something like:
//
#if 0
return Container::iterator(
	const_cast<Container::value_type *>(pointer_from(add_const(container))),
	iterator_constructor
);
#endif
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
template<typename T, typename Enable = void> \
struct has_rvalue_c_ ## begin_or_end : std::false_type {}; \
\
template<typename T> \
struct has_rvalue_c_ ## begin_or_end<T, decltype(rvalue_ref_qualified(&T::begin_or_end))> : std::true_type {}; \
\
template<typename T> \
constexpr bool has_rvalue_ ## begin_or_end = has_rvalue_c_ ## begin_or_end<T>::value; \
\
\
template<typename Container, BOUNDED_REQUIRES( \
	is_container<Container> and \
	std::is_rvalue_reference<Container &&>::value and \
	has_rvalue_ ## begin_or_end<Container> \
)> \
constexpr auto begin_or_end(Container && container) BOUNDED_NOEXCEPT_VALUE( \
	std::move(container).begin_or_end() \
) \
\
template<typename Container, BOUNDED_REQUIRES( \
	is_container<Container> and \
	std::is_rvalue_reference<Container &&>::value and \
	!has_rvalue_ ## begin_or_end<std::decay_t<Container>> \
)> \
constexpr auto begin_or_end(Container && container) BOUNDED_NOEXCEPT_VALUE( \
	::containers::move_iterator(begin_or_end(container)) \
)

CONTAINERS_DETAIL_BEGIN_END_OVERLOADS(begin)
CONTAINERS_DETAIL_BEGIN_END_OVERLOADS(end)


template<typename Container, BOUNDED_REQUIRES(is_container<Container>)>
constexpr auto cbegin(Container const & container) BOUNDED_NOEXCEPT_VALUE(
	begin(container)
)
template<typename Container, BOUNDED_REQUIRES(is_container<Container>)>
constexpr auto cend(Container const & container) BOUNDED_NOEXCEPT_VALUE(
	end(container)
)



template<typename Container, BOUNDED_REQUIRES(is_container<Container>)>
constexpr auto rbegin(Container && container) BOUNDED_NOEXCEPT_VALUE(
	::containers::reverse_iterator(begin(std::forward<Container>(container)))
)
template<typename Container, BOUNDED_REQUIRES(is_container<Container>)>
constexpr auto rend(Container && container) BOUNDED_NOEXCEPT_VALUE(
	::containers::reverse_iterator(end(std::forward<Container>(container)))
)



template<typename Container, BOUNDED_REQUIRES(is_container<Container>)>
constexpr auto crbegin(Container const & container) BOUNDED_NOEXCEPT_VALUE(
	::containers::reverse_iterator(begin(container))
)
template<typename Container, BOUNDED_REQUIRES(is_container<Container>)>
constexpr auto crend(Container const & container) BOUNDED_NOEXCEPT_VALUE(
	::containers::reverse_iterator(end(container))
)


#define CONTAINERS_COMMON_BEGIN_END_USING_DECLARATIONS \
	using ::containers::detail::common::begin; \
	using ::containers::detail::common::end; \
	using ::containers::detail::common::cbegin; \
	using ::containers::detail::common::cend; \
	using ::containers::detail::common::rbegin; \
	using ::containers::detail::common::rend; \
	using ::containers::detail::common::crbegin; \
	using ::containers::detail::common::crend;

}	// namespace common

CONTAINERS_COMMON_BEGIN_END_USING_DECLARATIONS

}	// namespace detail

CONTAINERS_COMMON_BEGIN_END_USING_DECLARATIONS

}	// namespace containers
