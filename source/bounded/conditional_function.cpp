// Copyright David Stone 2024.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

export module bounded.conditional_function;

namespace bounded {

export template<bool condition>
constexpr auto conditional_function(auto if_true, auto if_false) {
	if constexpr (condition) {
		return if_true;
	} else {
		return if_false;
	}
};

} // namespace bounded
