// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <bounded/detail/construct_destroy.hpp>
#include <bounded/detail/variant/visit.hpp>

#include <type_traits>

namespace bounded {

template<typename... Ts>
struct variant;

namespace detail {

template<typename... Ts>
struct variant_destructor {
	~variant_destructor() {
		visit(static_cast<variant<Ts...> &>(*this), destroy);
	}
};

template<typename... Ts> requires(... and std::is_trivially_destructible_v<Ts>)
struct variant_destructor<Ts...> {
};

}	// namespace detail
}	// namespace bounded
