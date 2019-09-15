// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <utility>

namespace bounded {

// This needs language support to work properly. This implementation does not
// work for function pointers or final function objects.
template<typename... Functions>
struct overload : Functions... {
	constexpr explicit overload(Functions... functions):
		Functions(std::move(functions))...
	{
	}
		
	using Functions::operator()...;
};

} // namespace bounded
