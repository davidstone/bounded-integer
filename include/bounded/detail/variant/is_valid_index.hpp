// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <bounded/detail/class.hpp>
#include <bounded/detail/comparison.hpp>
#include <bounded/detail/type.hpp>

namespace bounded {
namespace detail {

template<typename LHS, typename RHS>
constexpr auto types_equal(types<LHS> lhs, types<RHS> rhs) {
	return bounded::integer(lhs == rhs);
}

template<typename Index, typename... Ts>
concept matches_exactly_one_type_impl = (constant<0> + ... + ::bounded::detail::types_equal(Index(), Ts())) == constant<1>;

}	// namespace detail

template<typename Index, typename... Ts>
concept matches_exactly_one_type = detail::matches_exactly_one_type_impl<detail::types<std::decay_t<Index>>, Ts...>;

template<typename Index, typename... Ts>
concept variant_integer_index = bounded_integer<Index> and Index::value() < sizeof...(Ts);

template<typename Index, typename... Ts>
concept unique_type_identifier = detail::matches_exactly_one_type_impl<Index, Ts...> or variant_integer_index<Index, Ts...>;

}	// namespace bounded
