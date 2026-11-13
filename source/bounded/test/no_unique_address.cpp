// Copyright David Stone 2026.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

module;

#include <bounded/no_unique_address.hpp>

export module bounded.no_unique_address;

struct s {
	BOUNDED_NO_UNIQUE_ADDRESS int m;
};
