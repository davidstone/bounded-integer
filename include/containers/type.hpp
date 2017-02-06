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

using boost::hana::basic_type;
using boost::hana::type;
using boost::hana::type_c;

template<typename...>
struct types{};

}	// namespace detail
}	// namespace containers
