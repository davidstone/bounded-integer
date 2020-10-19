// Copyright David Stone 2020.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

namespace containers {

template<typename Range>
concept has_member_size = requires(Range const & range) { range.size(); };

} // namespace containers