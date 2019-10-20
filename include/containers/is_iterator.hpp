// Copyright David Stone 2015.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <iterator>
#include <type_traits>

namespace containers {

template<typename Iterator>
concept iterator = requires(Iterator it) {
	typename std::iterator_traits<Iterator>::iterator_category;
	++it;
	*it;
};

template<typename Iterator>
concept forward_iterator = iterator<Iterator> and std::is_copy_constructible_v<Iterator>;

template<typename Iterator>
concept bidirectional_iterator = forward_iterator<Iterator> and requires(Iterator it) { --it; };

// TODO: require bounded::ordered<Iterator> when more types support <=>
template<typename Iterator>
concept random_access_iterator = bidirectional_iterator<Iterator> and requires(Iterator it, typename std::iterator_traits<Iterator>::difference_type offset) {
	it - it;
	it + offset;
};

}	// namespace containers
