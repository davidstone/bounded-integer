// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <bounded/detail/class.hpp>
#include <bounded/detail/comparison.hpp>

namespace bounded {
namespace detail {

constexpr struct is_valid_index_t {
	template<auto n, typename... Ts>
	constexpr auto operator()(constant_t<n> index, types<Ts>...) const noexcept {
		return index < constant<sizeof...(Ts)>;
	}

	template<typename Index, typename... Ts>
	constexpr auto operator()(types<Index> index, types<Ts>... types) const noexcept {
		constexpr auto exactly_one_type_matches = (0 + ... + (index == types)) == 1;
		return exactly_one_type_matches;
	}
} is_valid_index;

}	// namespace detail
}	// namespace bounded
