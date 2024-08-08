// Copyright David Stone 2024.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

export module bounded.arithmetic.min_max;

namespace bounded {

export template<typename Min, typename Max>
struct min_max {
	Min min;
	Max max;
};

} // namespace bounded
