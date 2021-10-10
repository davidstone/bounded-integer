// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/begin_end.hpp>
#include <containers/is_iterator_sentinel.hpp>

#include <iterator>
#include <type_traits>

namespace containers {
namespace detail {

template<typename Range>
using iterator_t_impl = decltype(containers::begin(std::declval<Range &>()));

template<typename Range>
using sentinel_t = decltype(containers::end(std::declval<Range &>()));

} // namespace detail

template<typename T>
concept range = sentinel_for<detail::sentinel_t<T>, detail::iterator_t_impl<T>>;

template<typename T>
concept forward_range = range<T> and forward_iterator<detail::iterator_t_impl<T>>;

template<typename T>
concept bidirectional_range = forward_range<T> and bidirectional_iterator<detail::iterator_t_impl<T>>;

template<typename T>
concept forward_random_access_range = forward_range<T> and forward_random_access_iterator<detail::iterator_t_impl<T>>;

template<typename T>
concept random_access_range = bidirectional_range<T> and forward_random_access_range<T>;

}	// namespace containers
