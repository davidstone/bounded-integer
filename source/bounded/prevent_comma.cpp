// Copyright David Stone 2023.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

export module bounded.prevent_comma;

namespace bounded::detail {

export auto prevent_comma(auto &&) -> void;

} // namespace bounded::detail