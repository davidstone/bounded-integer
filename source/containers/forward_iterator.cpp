// Copyright David Stone 2023.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

export module containers.forward_iterator;

import containers.is_iterator;

import bounded;
import std_module;

namespace containers {

template<typename Iterator>
concept only_input_or_output_iterator =
	std::same_as<typename Iterator::iterator_category, std::input_iterator_tag> or
	std::same_as<typename Iterator::iterator_category, std::output_iterator_tag>;

export template<typename Iterator>
concept forward_iterator =
	iterator<Iterator> and
	bounded::copy_constructible<std::remove_cvref_t<Iterator>> and
	// This test is needed only to support legacy copyable iterators
	!only_input_or_output_iterator<std::remove_cvref_t<Iterator>>;

} // namespace containers
