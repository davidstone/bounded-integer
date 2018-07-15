// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/type.hpp>
#include <containers/variant/is_valid_index.hpp>

#include <bounded/integer.hpp>

namespace containers {
namespace detail {

constexpr struct get_index_t {
	template<typename Index, typename T, typename... Ts, BOUNDED_REQUIRES(is_valid_index(types<Index>{}, T{}, Ts{}...))>
	constexpr auto operator()(types<Index> index, T head, Ts... tail) const noexcept {
		if constexpr (index == head) {
			return 0_bi;
		} else {
			return 1_bi + operator()(index, tail...);
		}
	}
	template<auto n, typename... Ts, BOUNDED_REQUIRES(is_valid_index(bounded::constant<n>, Ts{}...))>
	constexpr auto operator()(bounded::constant_t<n> index, Ts...) const noexcept {
		return index;
	}
} get_index;


constexpr struct get_type_t {
	template<typename Index, typename... Ts, BOUNDED_REQUIRES(is_valid_index(types<Index>{}, Ts{}...))>
	constexpr auto operator()(types<Index> index, Ts...) const noexcept {
		return index;
	}

	template<auto n, typename T, typename... Ts, BOUNDED_REQUIRES(is_valid_index(bounded::constant<n>, T{}, Ts{}...))>
	constexpr auto operator()(bounded::constant_t<n> index, T head, Ts... tail) const noexcept {
		if constexpr (index == 0_bi) {
			return head;
		} else {
			return operator()(index - 1_bi, tail...);
		}
	}
} get_type;


}	// namespace detail
}	// namespace containers
