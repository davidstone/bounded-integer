// Copyright David Stone 2022.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/iterator_t.hpp>
#include <containers/range_value_t.hpp>

#include <bounded/concepts.hpp>

#include <concepts>

namespace containers::detail {

template<typename Container>
concept supports_lazy_insert_after =
	requires(Container container, bounded::function_ptr<range_value_t<Container>> make) {
		{ container.lazy_insert_after(container.before_begin(), make) } -> std::same_as<iterator_t<Container &>>;
	};

} // namespace containers::detail
