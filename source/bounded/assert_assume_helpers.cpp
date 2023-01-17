// Copyright David Stone 2023.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

export module bounded.assert_assume_helpers;

import bounded.concepts;

namespace bounded::detail {

auto non_constexpr() -> void {
}

export constexpr auto constexpr_only_if(explicitly_convertible_to<bool> auto const & condition) -> void {
	if (!condition) {
		non_constexpr();
	}
}

export auto prevent_comma(auto &&) -> void;

} // namespace bounded::detail