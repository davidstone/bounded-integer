// Copyright David Stone 2016.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

namespace bounded {

template<typename T>
struct optional;

// none_t cannot be default constructible or we get an ambiguity in op = {};
struct none_t {
	constexpr explicit none_t(int) {}
};
constexpr auto none = none_t(0);

}	// namespace bounded
