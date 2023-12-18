// Copyright David Stone 2023.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

export module containers.random_access_iterator;

import containers.bidirectional_iterator;
import containers.forward_random_access_iterator;
import containers.iter_difference_t;
import containers.iterator_addable;

import bounded;
import std_module;

namespace containers {

// TODO: require bounded::ordered<Iterator> when more types support <=>
export template<typename Iterator>
concept random_access_iterator =
	bidirectional_iterator<Iterator> and
	forward_random_access_iterator<Iterator> and
	iterator_addable<Iterator, iter_difference_t<Iterator>>;

} // namespace containers
