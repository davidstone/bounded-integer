// Copyright David Stone 2016.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

namespace containers {
namespace detail {

template<typename...>
using void_t = void;

template<typename...>
struct type_list{};

}	// namespace detail
}	// namespace containers
