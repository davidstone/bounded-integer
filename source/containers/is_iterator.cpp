// Copyright David Stone 2015.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

export module containers.is_iterator;

import containers.addable;
import containers.iter_difference_t;
import containers.offset_type;
import containers.subtractable;

import bounded;
import std_module;

namespace containers {

template<typename Iterator>
concept only_input_or_output_iterator =
	std::same_as<typename Iterator::iterator_category, std::input_iterator_tag> or
	std::same_as<typename Iterator::iterator_category, std::output_iterator_tag>;

export template<typename Iterator>
concept iterator = requires(std::remove_cvref_t<Iterator> it) {
	*it;
	++it;
};

export template<typename Iterator>
concept forward_iterator =
	iterator<Iterator> and
	bounded::copy_constructible<std::remove_cvref_t<Iterator>> and
	// This test is needed only to support legacy copyable iterators
	!only_input_or_output_iterator<std::remove_cvref_t<Iterator>>;

export template<typename Iterator>
concept bidirectional_iterator =
	forward_iterator<Iterator> and
	requires(std::remove_cvref_t<Iterator> it) { --it; };

export template<typename Iterator>
concept forward_random_access_iterator =
	forward_iterator<Iterator> and
	subtractable<Iterator> and
	addable<Iterator, offset_type<Iterator>>;

// TODO: require bounded::ordered<Iterator> when more types support <=>
export template<typename Iterator>
concept random_access_iterator = bidirectional_iterator<Iterator> and forward_random_access_iterator<Iterator> and requires(Iterator it, iter_difference_t<Iterator> offset) {
	it + offset;
};

} // namespace containers
