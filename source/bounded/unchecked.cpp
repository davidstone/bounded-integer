// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

export module bounded.unchecked;

namespace bounded {

export struct unchecked_t {
};
export constexpr auto unchecked = unchecked_t();

} // namespace bounded
