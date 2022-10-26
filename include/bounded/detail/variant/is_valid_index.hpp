// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <bounded/detail/class.hpp>
#include <bounded/detail/comparison.hpp>
#include <bounded/type.hpp>

namespace bounded {
namespace detail {

template<typename Index, typename... Ts>
concept matches_exactly_one_type_impl = (constant<0> + ... + bounded::integer(std::same_as<Index, Ts>)) == constant<1>;

}	// namespace detail

template<typename Index, typename... Ts>
concept matches_exactly_one_type = detail::matches_exactly_one_type_impl<std::decay_t<Index>, Ts...>;

template<typename Index, std::size_t size>
concept variant_integer_index = bounded_integer<Index> and Index::value() < size;

template<typename Index, typename... Ts>
concept unique_type_identifier = detail::matches_exactly_one_type_impl<typename Index::type, Ts...> or variant_integer_index<Index, sizeof...(Ts)>;

} // namespace bounded
