// Copyright David Stone 2022.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/is_iterator.hpp>

namespace containers::detail {

template<typename Iterator>
struct iterator_category_base {
};

template<only_input_or_output_iterator Iterator>
struct iterator_category_base<Iterator> {
	using iterator_category = typename Iterator::iterator_category;
};

} // namespace containers::detail
