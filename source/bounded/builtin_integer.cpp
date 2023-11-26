// Copyright David Stone 2023.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

export module bounded.builtin_integer;

import bounded.signed_builtin;
import bounded.unsigned_builtin;

namespace bounded {

export template<typename T>
concept builtin_integer =
	signed_builtin<T> or
	unsigned_builtin<T>;

} // namespace bounded