// Copyright David Stone 2016.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/is_iterator_sentinel.hpp>
#include <containers/type_list.hpp>

#include <iterator>

namespace containers {
namespace detail {

using std::begin;
using std::end;

template<typename T, typename = void>
struct has_begin_end_iterator_sentinel : std::false_type {};

// This has to be slightly more complicated than I would like due to
// https://stackoverflow.com/questions/22486386/partially-specializing-a-non-type-template-parameter-of-dependent-type
template<typename T>
struct has_begin_end_iterator_sentinel<
	T,
	void_t<decltype(begin(std::declval<T>())), decltype(end(std::declval<T>()))>
> : std::integral_constant<bool, is_iterator_sentinel<decltype(begin(std::declval<T>())), decltype(end(std::declval<T>()))>>{};

}	// namespace detail

using std::begin;
using std::end;

template<typename Iterable>
constexpr auto is_iterable = detail::has_begin_end_iterator_sentinel<Iterable>::value;

}	// namespace containers
