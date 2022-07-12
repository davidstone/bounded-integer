// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <bounded/detail/variant/is_valid_index.hpp>
#include <bounded/detail/arithmetic/operators.hpp>
#include <bounded/detail/class.hpp>
#include <bounded/detail/comparison.hpp>
#include <bounded/type.hpp>

#include <bounded/nth_type.hpp>

namespace bounded {
namespace detail {

struct get_index_t {
	template<typename Index, typename... Ts> requires matches_exactly_one_type<Index, typename Ts::type...>
	constexpr auto operator()(types<Index> index, Ts... ts) const {
		constexpr auto result = [=] {
			bool found = false;
			std::size_t value = 0;
			(
				...,
				(!found ?
					void(((found = (index == ts)), ++value)) :
					void()
				)
			);
			return value - 1;
		}();
		return constant<result>;
	}
	template<auto n, typename... Ts> requires variant_integer_index<constant_t<n>, sizeof...(Ts)>
	constexpr auto operator()(constant_t<n> index, Ts...) const {
		return index;
	}
} constexpr inline get_index;


struct get_type_t {
	template<typename Index, typename... Ts> requires matches_exactly_one_type<Index, typename Ts::type...>
	constexpr auto operator()(types<Index> index, Ts...) const {
		return index;
	}

	template<auto n, typename... Ts> requires variant_integer_index<constant_t<n>, sizeof...(Ts)>
	constexpr auto operator()(constant_t<n>, Ts...) const {
		return types<nth_type<n, typename Ts::type...>>();
	}
} constexpr inline get_type;


}	// namespace detail
}	// namespace bounded
