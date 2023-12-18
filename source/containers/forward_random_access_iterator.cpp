// Copyright David Stone 2023.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

export module containers.forward_random_access_iterator;

import containers.is_iterator;
import containers.iterator_addable;
import containers.offset_type;
import containers.subtractable;

import bounded;
import std_module;

namespace containers {

export template<typename Iterator>
concept forward_random_access_iterator =
	forward_iterator<Iterator> and
	subtractable<Iterator> and
	iterator_addable<Iterator, offset_type<Iterator>>;

} // namespace containers
