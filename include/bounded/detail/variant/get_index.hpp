// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <bounded/detail/variant/is_valid_index.hpp>
#include <bounded/detail/arithmetic/minus.hpp>
#include <bounded/detail/class.hpp>
#include <bounded/detail/comparison.hpp>
#include <bounded/detail/type.hpp>

namespace bounded {
namespace detail {

constexpr struct get_index_t {
	template<typename Index, typename T, typename... Ts, BOUNDED_REQUIRES(is_valid_index(types<Index>{}, T{}, Ts{}...))>
	constexpr auto operator()(types<Index> index, T head, Ts... tail) const noexcept {
		if constexpr (index == head) {
			return constant<0>;
		} else {
			return constant<1> + operator()(index, tail...);
		}
	}
	template<auto n, typename... Ts, BOUNDED_REQUIRES(is_valid_index(constant<n>, Ts{}...))>
	constexpr auto operator()(constant_t<n> index, Ts...) const noexcept {
		return index;
	}
} get_index;


constexpr struct get_type_t {
	template<typename Index, typename... Ts, BOUNDED_REQUIRES(is_valid_index(types<Index>{}, Ts{}...))>
	constexpr auto operator()(types<Index> index, Ts...) const noexcept {
		return index;
	}

	template<auto n, typename T, typename... Ts, BOUNDED_REQUIRES(is_valid_index(constant<n>, T{}, Ts{}...))>
	constexpr auto operator()(constant_t<n> index, T head, Ts... tail) const noexcept {
		if constexpr (index == constant<0>) {
			return head;
		} else {
			return operator()(index - constant<1>, tail...);
		}
	}
} get_type;


}	// namespace detail
}	// namespace bounded
