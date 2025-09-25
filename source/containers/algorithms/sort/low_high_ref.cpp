// Copyright David Stone 2023.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

export module containers.algorithms.sort.low_high_ref;

namespace containers {

export template<typename T>
struct low_high_ref {
	constexpr low_high_ref(T & low_, T & high_):
		low(low_),
		high(high_)
	{
	}

	T & low;
	T & high;
};

} // namespace containers
