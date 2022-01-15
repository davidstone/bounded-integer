// Copyright David Stone 2015.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/iter_difference_t.hpp>
#include <containers/offset_type.hpp>

#include <iterator>
#include <type_traits>

namespace containers {
namespace detail {

template<typename Iterator>
concept only_input_or_output_iterator =
	std::is_same_v<typename Iterator::iterator_category, std::input_iterator_tag> or
	std::is_same_v<typename Iterator::iterator_category, std::output_iterator_tag>;

} // namespace detail

template<typename Iterator>
concept iterator = requires(Iterator it) {
	*it;
	++it;
};

template<typename Iterator>
concept forward_iterator =
	iterator<Iterator> and
	std::is_copy_constructible_v<Iterator> and
	// This test is needed only to support legacy copyable iterators
	!detail::only_input_or_output_iterator<Iterator>;

template<typename Iterator>
concept bidirectional_iterator = forward_iterator<Iterator> and requires(Iterator it) { --it; };

template<typename Iterator>
concept forward_random_access_iterator = forward_iterator<Iterator> and requires(Iterator it, offset_type<Iterator> offset) {
	it - it;
	it + offset;
};

// TODO: require bounded::ordered<Iterator> when more types support <=>
template<typename Iterator>
concept random_access_iterator = bidirectional_iterator<Iterator> and forward_random_access_iterator<Iterator> and requires(Iterator it, iter_difference_t<Iterator> offset) {
	it + offset;
};

}	// namespace containers
