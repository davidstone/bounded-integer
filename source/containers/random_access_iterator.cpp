// Copyright David Stone 2023.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

export module containers.random_access_iterator;

import containers.forward_random_access_iterator;
import containers.is_iterator;
import containers.iter_difference_t;

import bounded;
import std_module;

namespace containers {

// TODO: require bounded::ordered<Iterator> when more types support <=>
export template<typename Iterator>
concept random_access_iterator =
	bidirectional_iterator<Iterator> and
	forward_random_access_iterator<Iterator> and
	requires(Iterator it, iter_difference_t<Iterator> offset) {
		{ it + offset } -> std::same_as<std::remove_cvref_t<Iterator>>;
	};

} // namespace containers
