// Copyright David Stone 2023.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

export module containers.algorithms.sort.cheaply_sortable;

import bounded;
import std_module;

using namespace bounded::literal;

namespace containers {

// Size determined experimentally, but also the size of a cache line. Maybe it
// should be `std::hardware_destructive_interference_size`?
// This incorrectly considers `std::string_view` cheaply_sortable. This ideally
// would have something like `has_default_compare`.
export template<typename T, typename Compare>
concept cheaply_sortable =
	std::is_trivially_copyable_v<T> and
	sizeof(T) <= 64_bi and
	(std::same_as<Compare, std::less<>> or std::same_as<Compare, std::greater<>>);

} // namespace containers