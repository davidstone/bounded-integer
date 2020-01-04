// Copyright David Stone 2020.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <bounded/detail/class.hpp>

namespace bounded {

// Another alternative would be to implement this in terms of
// std::array<std::byte, sizeof(T)>. That approach is more correct because it
// does not claim that there is a T object somewhere, but it would not work with
// constexpr. This means all of the users of tombstone_traits would have to have
// a separate code path guarded by `if (std::is_constant_evaluated())` to avoid
// the small object optimization somehow.

template<typename T>
struct tombstone_traits {
	static constexpr auto spare_representations = constant<0>;

	static auto make(auto index) -> T = delete;
	static auto index(T const &) = delete;
};

}	// namespace bounded