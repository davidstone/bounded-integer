// Copyright David Stone 2023.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

export module bounded.function_ptr;

namespace bounded {

export template<typename Result, typename... Args>
using function_ptr = Result(*)(Args...);

} // namespace bounded
