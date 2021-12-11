// Copyright David Stone 2019.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/begin_end.hpp>
#include <containers/is_iterator_sentinel.hpp>
#include <containers/is_range.hpp>
#include <containers/legacy_iterator.hpp>

#include <algorithm>
#include <functional>

namespace containers {

constexpr inline struct sort_t {
	template<iterator Iterator>
	constexpr void operator()(Iterator const first, sentinel_for<Iterator> auto const last, auto cmp) const {
		std::sort(make_legacy_iterator(first), make_legacy_iterator(last), cmp);
	}
	constexpr void operator()(range auto & to_sort, auto cmp) const {
		operator()(containers::begin(to_sort), containers::end(to_sort), cmp);
	}
	constexpr void operator()(range auto & to_sort) const {
		operator()(to_sort, std::less{});
	}
} sort;

} // namespace containers
