// Copyright David Stone 2023.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

export module containers.algorithms.sort.rotate_one;

import containers.reference_wrapper;

import bounded;
import tv;

namespace containers {

export constexpr auto rotate_one(auto & arg, auto & ... args) {
	auto storage = tv::relocate_into_storage(arg);
	auto ref = ::containers::make_reference_wrapper(arg);
	(..., (bounded::relocate_at(ref.get(), args), ref = args));
	bounded::relocate_at(ref.get(), storage.value);
}

} // namespace containers

static_assert([]{
	int a = 0;
	int b = 1;
	int c = 2;
	int d = 3;
	::containers::rotate_one(d, c, b, a);
	return a == 3 and b == 0 and c == 1 and d == 2;
}());