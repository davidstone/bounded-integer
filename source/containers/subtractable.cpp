// Copyright David Stone 2023.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

export module containers.subtractable;

namespace containers {

export template<typename Iterator>
concept subtractable = requires(Iterator const it) { it - it; };

} // namespace containers
