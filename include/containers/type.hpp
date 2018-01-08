// Copyright David Stone 2017.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <boost/hana/type.hpp>

namespace containers {
namespace detail {

template<typename...>
using void_t = void;

template<typename...>
struct types{};

}	// namespace detail


template<typename T>
using basic_type = boost::hana::basic_type<T>;

template<typename T>
using type = boost::hana::type<T>;

using boost::hana::type_c;

}	// namespace containers
