// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <bounded/detail/construct_destroy.hpp>
#include <bounded/detail/variant/visit.hpp>

#include <type_traits>

namespace bounded {

template<typename GetFunction, typename... Ts>
struct basic_variant;

namespace detail {

template<typename GetFunction, typename... Ts>
struct variant_destructor {
	~variant_destructor() {
		visit(static_cast<basic_variant<GetFunction, Ts...> &>(*this), destroy);
	}
};

template<typename GetFunction, typename... Ts> requires(... and std::is_trivially_destructible_v<Ts>)
struct variant_destructor<GetFunction, Ts...> {
};

}	// namespace detail
}	// namespace bounded
